/**
 * @file Updater.cpp
 * @author Ronny Antoon
 */

#include <stddef.h>                        // size_t
#include <stdint.h>                        // uint8_t
#include <esp32-hal-gpio.h>                // HIGH, LOW
#include <esp_partition.h>                 // esp_partition_t
#include <esp_ota_ops.h>                   // esp_ota_set_boot_partition
#include <Esp.h>                           // ESP.getFreeSketchSpace
#include <Stream.h>                        // Stream
#include <MultiPrinterLoggerInterface.hpp> // logger

#include "Updater.hpp"

Updater::Updater(uint8_t pin, bool ledOnHigh, MultiPrinterLoggerInterface *logger)
{
    _pin = pin;               // Pin Number for the LED.
    _ledOnHigh = ledOnHigh;   // Set to true if the LED is on when the pin is HIGH.
    _onStart_cb = nullptr;    // Callback function to be invoked at the start of an update.
    _onProgress_cb = nullptr; // Callback function to be invoked during the update progress.
    _onEnd_cb = nullptr;      // Callback function to be invoked at the end of an update.
    _onError_cb = nullptr;    // Callback function to be invoked in case of an error during the update.
    _streamData = nullptr;    // Stream object to read the data from.
    _newPartition = nullptr;  // Pointer to the partition to be updated.
    _updateTimer = 0;         // Timer to measure the update duration.
    _logger = logger;         // Logger object to log messages.

    // Initialize the LED pin if it is provided and set it to OFF.
    if (_pin != -1)
    {
        pinMode(_pin, OUTPUT);
        digitalWrite(_pin, !_ledOnHigh);
    }
}

// Method to set the callback function to be invoked at the start of an update.
void Updater::setOnStartFunc(void (*onStart_cb)())
{
    _onStart_cb = onStart_cb;
}

// Method to set the callback function to be invoked during the update progress.
void Updater::setOnProgressFunc(void (*onProgress_cb)(int, int))
{
    _onProgress_cb = onProgress_cb;
}

// Method to set the callback function to be invoked at the end of an update.
void Updater::setOnEndFunc(void (*onEnd_cb)())
{
    _onEnd_cb = onEnd_cb;
}

// Method to set the callback function to be invoked in case of an error during the update.
void Updater::setOnErrorFunc(void (*onError_cb)(int))
{
    _onError_cb = onError_cb;
}

UPDATER_ERROR Updater::startUpdate(Stream *streamData, int streamLength, UPDATER_TYPE type)
{
    _streamData = streamData;     // Set the input stream for the update.
    _streamLength = streamLength; // Set the length of the input stream.

    // Check if there is enough free space for the update. According to the ESP-IDF documentation, the minimum free space
    uint32_t maxSketchSpace = ESP.getFreeSketchSpace() - (ESP.getFreeSketchSpace() % BLOCK_SIZE_P);
    if (_streamLength > maxSketchSpace)
        return _abort(UPDATER_ERROR::NO_ENOUGH_SPACE);

    UPDATER_ERROR err = UPDATER_ERROR::OK;

    // Determine the type of update and get the partition accordingly.
    if (type == UPDATER_TYPE::FIRMWARE)
        err = getAPPUpdatePartition();
    else if (type == UPDATER_TYPE::SPIFFS)
        err = getSPIFFSPartition();
    else
        err = UPDATER_ERROR::UNKNOWN;

    // Check if the partition type is valid.
    if (err != UPDATER_ERROR::OK)
        return _abort(err);

    callOnStart();                    // Invoke the callback for update start.
    callOnProgress(0, _streamLength); // Invoke the callback for update progress.

    size_t written = 0; // Variable to keep track of the number of bytes written.
    size_t toWrite = 0; // Variable to keep track of the number of bytes to write.

    while (written < _streamLength) // Loop until all the bytes are written.
    {
        toggleLed(); // Toggle the LED to indicate the update is in progress.

        resetBuffer(BLOCK_SIZE_P); // Clear the buffer to prepare for the next block.

        // Read the next block from the input stream. And check if the read operation was successful.
        toWrite = readBlockFirmwareToBuffer(written, BLOCK_SIZE_P); // Read the next block from the input stream.
        if (toWrite <= 0)
        {
            Log_Error(_logger, "Can't read from firmware!");
            return _abort(UPDATER_ERROR::READ_FAILED);
        }

        toggleLed(); // Toggle the LED to indicate the update is in progress.

        // Erase the partition range to prepare for the next block. And check if the erase operation was successful.
        err = resetPartitionRange(written, BLOCK_SIZE_P);
        if (err != UPDATER_ERROR::OK)
            return _abort(err);

        // Check if this is the last block for the stream and the partition has enough space for another block. erase the next block.
        if (_streamLength == written + BLOCK_SIZE_P && _streamLength < maxSketchSpace - BLOCK_SIZE_P)
        {
            err = resetPartitionRange(written + BLOCK_SIZE_P, BLOCK_SIZE_P);
            if (err != UPDATER_ERROR::OK)
                return _abort(err);
        }

        // Write the block to the partition. And check if the write operation was successful.
        err = writeBlockBufferToPartition(written, toWrite);
        if (err != UPDATER_ERROR::OK)
            return _abort(err);

        written += toWrite;                     // Update the number of bytes written.
        callOnProgress(written, _streamLength); // Invoke the callback for update progress.
    }

    // Check if the update is for the firmware and change the boot partition.
    if (type == UPDATER_TYPE::FIRMWARE)
        err = changeBootPartition();

    // Check if there was an error during the update.
    if (err != UPDATER_ERROR::OK)
        return _abort(err);

    callOnEnd(); // Invoke the callback for update end.
    return UPDATER_ERROR::OK;
}

// Method to invoke the callback for update start.
void Updater::callOnStart()
{
    _updateTimer = millis(); // Start the timer to measure the update duration.
    Log_Debug(_logger, "Update started");
    if (_onStart_cb)
    {
        _onStart_cb();
    }
}

// Method to invoke the callback for update progress.
void Updater::callOnProgress(int _wretten, int _length)
{
    Log_Debug(_logger, "Progress: %d%%", (_wretten * 100) / _length);
    if (_onProgress_cb)
    {
        _onProgress_cb(_wretten, _length);
    }
}

// Method to invoke the callback for update completion.
void Updater::callOnEnd()
{
    Log_Debug(_logger, "Update finished, The update took: %.2f seconds", (double)(millis() - _updateTimer) / 1000);
    if (_onEnd_cb)
    {
        _onEnd_cb();
    }
}

// Method to invoke the callback for an update error.
void Updater::callOnError(int _error)
{
    if (_onError_cb)
    {
        _onError_cb(_error);
    }
}
// Method to read a block of firmware data into a buffer.
size_t Updater::readBlockFirmwareToBuffer(size_t offset, size_t length)
{
    // Check if this is the last block. If so, set the length to the remaining bytes.
    if (_streamLength < offset + length)
    {
        length = _streamLength - offset;
    }

    size_t readed = 0;  // Variable to keep track of the number of bytes readed.
    uint16_t retry = 0; // Variable to keep track of the number of retries.

    // Read the block from the input stream.
    readed = _streamData->readBytes(_buffer, length);
    retry++;

    // Check if the read operation was successful. If not, retry up to 30 times. With a delay of 100ms between retries.
    while (readed == 0 && retry < 30)
    {
        Log_Debug(_logger, "Reading block from firmware, offset: %d, length: %d. Got delayed because unkown issue.", offset, length);
        delay(100);
        readed = _streamData->readBytes(_buffer, length);
        retry++;
    }

    // Check if the read operation was successful.
    if (readed <= 0)
        return 0;

    return readed; // Return the number of bytes readed.
}

UPDATER_ERROR Updater::writeBlockBufferToPartition(size_t offset, size_t length)
{
    esp_err_t err = esp_partition_write(_newPartition, offset, _buffer, length);
    if (err != ESP_OK)
    {
        Log_Error(_logger, "Can't write to partition!");
        return UPDATER_ERROR::PARTITION_WRITE_FAILED;
    }
    return UPDATER_ERROR::OK;
}

// Method to change the boot partition to the updated partition.
UPDATER_ERROR Updater::changeBootPartition()
{
    // Set the updated partition as the boot partition. And check if the operation was successful.
    esp_err_t err = esp_ota_set_boot_partition(_newPartition);
    if (err != ESP_OK)
    {
        Log_Error(_logger, "Change boot partition failed!");
        return UPDATER_ERROR::CHANGE_BOOT_PARTITION_FAILED;
    }
    return UPDATER_ERROR::OK;
}

// Method to toggle the LED.
void Updater::toggleLed()
{
    if (_pin == -1)
        return;
    digitalWrite(_pin, _ledOnHigh); // Set the LED to the desired state.
    _ledOnHigh = !_ledOnHigh;       // Toggle the LED state.
}

// Method to clear the buffer.
void Updater::resetBuffer(size_t length)
{
    for (uint32_t i = 0; i < length; i++)
    {
        _buffer[i] = 0;
    }
}

// Method to erase a range of the partition.
UPDATER_ERROR Updater::resetPartitionRange(size_t offset, size_t length)
{
    // Erase the partition range. And check if the operation was successful.
    esp_err_t err = esp_partition_erase_range(_newPartition, offset, length);
    if (err != ESP_OK)
    {
        Log_Error(_logger, "Can't erase partition!");
        return UPDATER_ERROR::UNKNOWN;
    }
    return UPDATER_ERROR::OK;
}

// Method to get the next updatable partition.
UPDATER_ERROR Updater::getAPPUpdatePartition()
{
    // Get the next updatable partition. And check if there is a partition available for update.
    _newPartition = esp_ota_get_next_update_partition(NULL);
    if (_newPartition == NULL)
    {
        Log_Error(_logger, "There is no updatable partition.");
        return UPDATER_ERROR::NO_PARTITION_AVAILABLE;
    }
    return UPDATER_ERROR::OK;
}

// Method to get the SPIFFS partition.
UPDATER_ERROR Updater::getSPIFFSPartition()
{
    // Get the SPIFFS partition. And check if there is a partition available for update.
    _newPartition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_SPIFFS, NULL);
    if (_newPartition == NULL)
    {
        Log_Error(_logger, "There is no SPIFFS partition.");
        return UPDATER_ERROR::NO_PARTITION_AVAILABLE;
    }
    return UPDATER_ERROR::OK;
}

// Method to abort the update process.
UPDATER_ERROR Updater::_abort(UPDATER_ERROR error)
{
    callOnError((int)error);
    return error;
}
