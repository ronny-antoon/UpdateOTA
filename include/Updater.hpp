#ifndef UPDATER_HPP
#define UPDATER_HPP

/**
 * @file Updater.hpp
 * @brief Class for performing updates on ESP32 devices.
 * @author Ronny Antoon
 *
 * This class provides methods for performing firmware and SPIFFS updates on ESP32 devices.
 */

#include <stdint.h>         // uint8_t
#include <esp32-hal-gpio.h> // HIGH, LOW
#include <esp_partition.h>  // esp_partition_t
#include <Stream.h>         // Stream

#include "UpdaterInterface.hpp"
/**
 * @brief Updater class for performing firmware and SPIFFS updates on ESP32.
 *
 * This class provides methods for updating firmware and SPIFFS partitions on an ESP32 device.
 */
class Updater : public UpdaterInterface
{
public:
    /**
     * @brief Constructor for the Updater class.
     *
     * @param pin The GPIO pin number connected to an LED indicator.
     * @param ledOnHigh Set to `true` if the LED is on when the pin is HIGH.
     */
    Updater(uint8_t pin = 2, bool ledOnHigh = HIGH);

    /**
     * @brief Destructor for the Updater class.
     */
    ~Updater() override{};

    /**
     * @brief Set a callback function to be invoked at the start of an update.
     *
     * @param onStart_cb A function pointer to the callback function.
     */
    void setOnStartFunc(void (*onStart_cb)()) override;

    /**
     * @brief Set a callback function to be invoked during the update progress.
     *
     * @param onProgress_cb A function pointer to the callback function.
     */
    void setOnProgressFunc(void (*onProgress_cb)(int, int)) override;

    /**
     * @brief Set a callback function to be invoked at the end of an update.
     *
     * @param onEnd_cb A function pointer to the callback function.
     */
    void setOnEndFunc(void (*onEnd_cb)()) override;

    /**
     * @brief Set a callback function to be invoked in case of an error during the update.
     *
     * @param onError_cb A function pointer to the callback function.
     */
    void setOnErrorFunc(void (*onError_cb)(int)) override;

    /**
     * @brief Start the update process.
     *
     * @param streamData A pointer to a Stream object containing update data.
     * @param streamLength The length of the update data stream.
     * @param type The type of update (FIRMWARE or SPIFFS).
     * @return UPDATER_ERROR An error code indicating the result of the update process:
     *          - UPDATER_ERROR::OK: Update completed successfully.
     *          - UPDATER_ERROR::NO_PARTITION_AVAILABLE: No partition available for update.
     *          - UPDATER_ERROR::CHANGE_BOOT_PARTITION_FAILED: Failed to change the boot partition.
     *          - UPDATER_ERROR::PARTITION_WRITE_FAILED: Failed to write to the partition.
     *          - UPDATER_ERROR::READ_FAILED: Failed to read from the update data.
     *          - UPDATER_ERROR::NO_ENOUGH_SPACE: Not enough space for the update.
     *          - UPDATER_ERROR::UNKNOWN: An unknown error occurred during the update.
     */
    UPDATER_ERROR startUpdate(Stream *streamData, int streamLength, UPDATER_TYPE type) override;

private:
    // Private methods

    /**
     * @brief Invoke the callback for update start.
     */
    void callOnStart();

    /**
     * @brief Invoke the callback for update progress.
     *
     * @param _written The number of bytes written.
     * @param _length The total length of the update data.
     */
    void callOnProgress(int _written, int _length);

    /**
     * @brief Invoke the callback for update completion.
     */
    void callOnEnd();

    /**
     * @brief Invoke the callback for an update error.
     *
     * @param _error The error code indicating the type of error.
     */
    void callOnError(int _error);

    /**
     * @brief Read a block of firmware data into a buffer.
     *
     * @param offset The offset within the update data stream.
     * @param length The length of the block to read.
     * @return The number of bytes read into the buffer.
     */
    size_t readBlockFirmwareToBuffer(size_t offset, size_t length);

    /**
     * @brief Write a block from the buffer to the update partition.
     *
     * @param offset The offset within the partition.
     * @param length The length of the block to write.
     * @return UPDATER_ERROR An error code indicating the result of the write operation:
     *          - UPDATER_ERROR::OK: Write operation completed successfully.
     *          - UPDATER_ERROR::PARTITION_WRITE_FAILED: Failed to write to the partition.
     */
    UPDATER_ERROR writeBlockBufferToPartition(size_t offset, size_t length);

    /**
     * @brief Change the boot partition to the updated partition.
     *
     * @return UPDATER_ERROR An error code indicating the result of the operation:
     *          - UPDATER_ERROR::OK: Boot partition changed successfully.
     *          - UPDATER_ERROR::CHANGE_BOOT_PARTITION_FAILED: Failed to change the boot partition.
     */
    UPDATER_ERROR changeBootPartition();

    /**
     * @brief Toggle the LED indicator.
     */
    void toggleLed();

    /**
     * @brief Reset the buffer by clearing its contents.
     *
     * @param length The length of the buffer to reset.
     */
    void resetBuffer(size_t length);

    /**
     * @brief Erase a range of the update partition.
     *
     * @param offset The offset within the partition to start erasing.
     * @param length The length of the range to erase.
     * @return UPDATER_ERROR An error code indicating the result of the erase operation:
     *          - UPDATER_ERROR::OK: Erase operation completed successfully.
     *          - UPDATER_ERROR::UNKNOWN: An unknown error occurred during the erase operation.
     */
    UPDATER_ERROR resetPartitionRange(size_t offset, size_t length);

    /**
     * @brief Get the next updatable partition for firmware updates.
     *
     * @return UPDATER_ERROR An error code indicating the result of the operation:
     *          - UPDATER_ERROR::OK: Partition found successfully.
     *          - UPDATER_ERROR::NO_PARTITION_AVAILABLE: No partition available for update.
     */
    UPDATER_ERROR getAPPUpdatePartition();

    /**
     * @brief Get the SPIFFS partition for SPIFFS updates.
     *
     * @return UPDATER_ERROR An error code indicating the result of the operation:
     *          - UPDATER_ERROR::OK: SPIFFS partition found successfully.
     *          - UPDATER_ERROR::NO_PARTITION_AVAILABLE: No SPIFFS partition available for update.
     */
    UPDATER_ERROR getSPIFFSPartition();

    /**
     * @brief Abort the update process and invoke the error callback.
     *
     * @param error The error code indicating the type of error.
     * @return UPDATER_ERROR The same error code that was passed as an argument.
     */
    UPDATER_ERROR _abort(UPDATER_ERROR error);

    uint8_t _pin;                         // GPIO pin number connected to an LED indicator
    bool _ledOnHigh;                      // Set to `true` if the LED is on when the pin is HIGH
    Stream *_streamData;                  // Pointer to a Stream object containing update data
    int _streamLength;                    // The length of the update data stream
    const esp_partition_t *_newPartition; // Pointer to the new partition to update
    char _buffer[BLOCK_SIZE_P];           // Buffer for storing update data
    unsigned long _updateTimer;           // Timer for measuring update progress

    // Function pointer to the callback function
    void (*_onStart_cb)();
    void (*_onProgress_cb)(int, int);
    void (*_onEnd_cb)();
    void (*_onError_cb)(int);

    const char *TAG = "Updater"; // Tag for logging
};

#endif // UPDATER_HPP