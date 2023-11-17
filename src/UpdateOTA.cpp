#include "UpdateOTA.hpp"

UpdateOTA::UpdateOTA(MultiPrinterLoggerInterface *logger) : _logger(logger)
{
    Log_Debug(_logger, "UpdateOTA constructor");
    // Initialize member variables
    _newPartition = nullptr;
    _isFirmware = false;
    _pinStatus = 0;
    _uRL = nullptr;
    _httpCode = 0;
    _wifiClientSecure = nullptr;
    _httpClient = nullptr;
}

UpdateOTA::~UpdateOTA()
{
    Log_Debug(_logger, "UpdateOTA destructor");
    // Clean up resources on destruction
    if (_httpClient != nullptr)
    {
        _httpClient->end();
        delete _httpClient;
        _httpClient = nullptr;
    }
    if (_wifiClientSecure != nullptr)
    {
        delete _wifiClientSecure;
        _wifiClientSecure = nullptr;
    }
}

UpdateOTAError UpdateOTA::startUpdate(const char *uRL, bool isFirmware, uint8_t pinStatus)
{
    // Set member variables based on input parameters
    _uRL = uRL;
    _isFirmware = isFirmware;
    _pinStatus = pinStatus;

    // Check if the device is connected to the internet
    if (WiFi.status() != WL_CONNECTED)
        return UpdateOTAError::NO_INTERNET;

    // Initialize WiFiClientSecure and HTTPClient
    _wifiClientSecure = new WiFiClientSecure();
    _httpClient = new HTTPClient();
    UpdateOTAError err = UpdateOTAError::SUCCESS;

    // Process the GET request
    err = processGetRequest();
    if (err != UpdateOTAError::SUCCESS)
        return err;

    // Check if there is enough space for the firmware
    uint64_t maxSketchSpace = ESP.getFreeSketchSpace() - (ESP.getFreeSketchSpace() % BLOCK_SIZE_P);
    if (_httpClient->getSize() > maxSketchSpace)
        return UpdateOTAError::NO_ENOUGH_SPACE;

    // Get the next updatable partition and check if there is a partition available for update
    err = selectPartition();
    if (err != UpdateOTAError::SUCCESS)
        return err;

    // Update the firmware
    err = updateFirmware();
    if (err != UpdateOTAError::SUCCESS)
        return err;

    // If the update is not for the firmware, then return
    if (!_isFirmware)
        return UpdateOTAError::SUCCESS;

    // Change the boot partition to the new partition
    err = changeBootPartition();
    if (err != UpdateOTAError::SUCCESS)
        return err;

    // Restart the ESP
    ESP.restart();

    // Never reached
    return UpdateOTAError::SUCCESS;
}

UpdateOTAError UpdateOTA::getVersionNumber(const char *uRL, char *buffer, uint8_t bufferSize)
{
    // Check if the device is connected to the internet
    if (WiFi.status() != WL_CONNECTED)
        return UpdateOTAError::NO_INTERNET;

    // Set member variables based on input parameters
    _uRL = uRL;
    _wifiClientSecure = new WiFiClientSecure();
    _httpClient = new HTTPClient();
    UpdateOTAError err = UpdateOTAError::SUCCESS;

    // Process the GET request
    err = processGetRequest();
    if (err != UpdateOTAError::SUCCESS)
        return err;

    // Check if there is enough space for the firmware
    if (_httpClient->getSize() > bufferSize)
        return UpdateOTAError::NO_ENOUGH_SPACE;

    // Read bytes directly into the buffer and null-terminate it
    _wifiClientSecure->readBytes(buffer, _httpClient->getSize());

    buffer[_httpClient->getSize()] = '\0'; // Null-terminate the string

    return UpdateOTAError::SUCCESS;
}

void UpdateOTA::errorToString(UpdateOTAError error, char *buffer, uint8_t bufferSize)
{
    // Convert an update OTA error to a human-readable string and store it in the provided buffer
    switch (error)
    {
    case UpdateOTAError::SUCCESS:
        strncpy(buffer, "No error occurred.", bufferSize);
        break;
    case UpdateOTAError::NO_INTERNET:
        strncpy(buffer, "No internet connection.", bufferSize);
        break;
    case UpdateOTAError::BAD_REQUEST:
        strncpy(buffer, "Bad request received.", bufferSize);
        break;
    case UpdateOTAError::UNAUTHORIZED:
        strncpy(buffer, "Unauthorized access.", bufferSize);
        break;
    case UpdateOTAError::PAGE_NOT_FOUND:
        strncpy(buffer, "Page(URL) not found.", bufferSize);
        break;
    case UpdateOTAError::NO_PARTITION_AVAILABLE:
        strncpy(buffer, "No partition available for update.", bufferSize);
        break;
    case UpdateOTAError::PARTITION_NOT_BOOTABLE:
        strncpy(buffer, "Selected partition is not bootable.", bufferSize);
        break;
    case UpdateOTAError::UPDATE_PROGRESS_ERROR:
        strncpy(buffer, "Error in update progress.", bufferSize);
        break;
    case UpdateOTAError::NO_ENOUGH_SPACE:
        strncpy(buffer, "Insufficient space for update.", bufferSize);
        break;
    default:
        strncpy(buffer, "Unknown error.", bufferSize);
        break;
    }
}

UpdateOTAError UpdateOTA::processGetRequest()
{
    // Process a GET request for the update version
    _wifiClientSecure->setCACert(ca_cert);
    _wifiClientSecure->setTimeout(30000); // Set the timeout for the input stream to 30 seconds.

    _httpClient->begin(*_wifiClientSecure, _uRL);
    _httpClient->setTimeout(10000);
    _httpClient->setFollowRedirects(followRedirects_t::HTTPC_DISABLE_FOLLOW_REDIRECTS);
    _httpClient->setUserAgent("RonnyAgend"); // TODO: change this to your own user agent
    _httpClient->addHeader("Cache-Control", "no-cache");

    _httpCode = _httpClient->GET();

    switch (_httpCode)
    {
    case HTTP_CODE_OK:
        return UpdateOTAError::SUCCESS;
    case HTTP_CODE_NOT_FOUND:
        return UpdateOTAError::PAGE_NOT_FOUND;
    case HTTP_CODE_UNAUTHORIZED:
        return UpdateOTAError::UNAUTHORIZED;
    case HTTP_CODE_BAD_REQUEST:
        return UpdateOTAError::BAD_REQUEST;
    default:
        break;
    }
    return UpdateOTAError::UNKNOWN;
}

UpdateOTAError UpdateOTA::updateFirmware()
{
    // Update the firmware
    if (_pinStatus != 0)
    {
        pinMode(_pinStatus, OUTPUT);    // Set the pin to output mode.
        digitalWrite(_pinStatus, HIGH); // Turn on the LED.
    }
    size_t written = 0; // Variable to keep track of the number of bytes written.
    size_t toWrite = 0; // Variable to keep track of the number of bytes to write.
    uint32_t _streamLength = _httpClient->getSize();

    while (written < _streamLength) // Loop until all the bytes are written.
    {
        printProgress(written, _streamLength); // Print the progress.

        resetBuffer(); // Clear the buffer to prepare for the next block.

        toggleLed(); // Toggle the LED.

        toWrite = readBlockFromClientToBuffer(written, BLOCK_SIZE_P); // Read the next block from the input stream.

        resetPartitionRange(written, BLOCK_SIZE_P); // Clear the partition range to prepare for the next block.

        toggleLed(); // Toggle the LED.

        writeBlockBufferToPartition(written, toWrite); // Write the block from the buffer to the partition.

        written += toWrite; // Update the number of bytes written.
    }

    printProgress(written, _streamLength); // Print the progress.
    _httpClient->end();                    // Close the input stream.
    if (_pinStatus != 0)
        digitalWrite(_pinStatus, LOW); // Turn off the LED.

    if (_streamLength != written)
        return UpdateOTAError::UPDATE_PROGRESS_ERROR; // Check if the number of bytes written is equal to the stream length. If not return an error.

    return UpdateOTAError::SUCCESS;
}

void UpdateOTA::resetBuffer()
{
    // Reset the buffer
    for (uint32_t i = 0; i < BLOCK_SIZE_P; i++)
    {
        _buffer[i] = 0;
    }
}

void UpdateOTA::resetPartitionRange(size_t offset, size_t length)
{
    // Reset the partition range
    esp_partition_erase_range(_newPartition, offset, length);
}

void UpdateOTA::writeBlockBufferToPartition(size_t offset, size_t length)
{
    // Write the block buffer to the partition
    esp_partition_write(_newPartition, offset, _buffer, length);
}

size_t UpdateOTA::readBlockFromClientToBuffer(size_t offset, size_t length)
{
    // Read a block from the client to the buffer
    if (_httpClient->getSize() < offset + length)
    {
        length = _httpClient->getSize() - offset;
    }

    size_t readed = 0;                                      // Variable to keep track of the number of bytes readed.
    readed = _wifiClientSecure->readBytes(_buffer, length); // Read the next block from the input stream.

    return readed;
}

UpdateOTAError UpdateOTA::changeBootPartition()
{
    // Change the boot partition
    if (esp_ota_set_boot_partition(_newPartition) != ESP_OK)
        return UpdateOTAError::PARTITION_NOT_BOOTABLE;
    return UpdateOTAError::SUCCESS;
}

UpdateOTAError UpdateOTA::selectPartition()
{
    // Select the partition for update
    if (_isFirmware)
    {
        _newPartition = esp_ota_get_next_update_partition(NULL);
    }
    else
    {
        _newPartition = esp_partition_find_first(ESP_PARTITION_TYPE_DATA, ESP_PARTITION_SUBTYPE_DATA_SPIFFS, NULL);
    }

    if (_newPartition == NULL)
        return UpdateOTAError::NO_PARTITION_AVAILABLE;

    return UpdateOTAError::SUCCESS;
}

void UpdateOTA::printProgress(size_t written, size_t total)
{
    // Print the update progress
    Log_Debug(_logger, "Progress: %d%%", (100 * written) / total);
}

void UpdateOTA::toggleLed()
{
    // Toggle the LED
    if (_pinStatus != 0)
        digitalWrite(_pinStatus, !digitalRead(_pinStatus));
}
