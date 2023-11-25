#include "UpdateOTA.hpp"

UpdateOTA::UpdateOTA(MultiPrinterLoggerInterface *logger, RelayModuleInterface *relayModule)
    : _logger(logger),
      _relayModule(relayModule)
{
    Log_Debug(_logger, "UpdateOTA created");
    // Initialize member variables
    _newPartition = nullptr;
    _isFirmware = false;
    _uRL = nullptr;
    _httpCode = 0;
    _wifiClientSecure = nullptr;
    _httpClient = nullptr;
}

UpdateOTA::~UpdateOTA()
{
    Log_Debug(_logger, "UpdateOTA destroyed");
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

UpdateOTAError UpdateOTA::startUpdate(const char *uRL, bool isFirmware)
{
    Log_Info(_logger, "UpdateOTA startUpdate: URL='%s', isFirmware=%s", uRL, isFirmware ? "true" : "false");

    // Set member variables based on input parameters
    _uRL = uRL;
    _isFirmware = isFirmware;

    // Check if the device is connected to the internet
    if (WiFi.status() != WL_CONNECTED)
    {
        Log_Error(_logger, "UpdateOTA startUpdate error: No internet connection");
        return UpdateOTAError::NO_INTERNET;
    }

    // Initialize WiFiClientSecure and HTTPClient
    _wifiClientSecure = new WiFiClientSecure();
    _httpClient = new HTTPClient();
    UpdateOTAError err = UpdateOTAError::SUCCESS;

    // Process the GET request
    err = processGetRequest();
    if (err != UpdateOTAError::SUCCESS)
    {
        Log_Error(_logger, "UpdateOTA startUpdate error: Failed to process GET request, ErrorCode=%d", err);
        return err;
    }

    // Check if there is enough space for the firmware
    uint64_t maxSketchSpace = ESP.getFreeSketchSpace() - (ESP.getFreeSketchSpace() % BLOCK_SIZE_P);
    if (_httpClient->getSize() > maxSketchSpace)
        return UpdateOTAError::NO_ENOUGH_SPACE;

    // Get the next updatable partition and check if there is a partition available for update
    err = selectPartition();
    if (err != UpdateOTAError::SUCCESS)
    {
        Log_Error(_logger, "UpdateOTA startUpdate error: Insufficient space for update");
        return err;
    }

    // Update the firmware
    err = updateFirmware();
    if (err != UpdateOTAError::SUCCESS)
    {
        Log_Error(_logger, "UpdateOTA startUpdate error: Failed to update firmware, ErrorCode=%d", err);
        return err;
    }

    // If the update is not for the firmware, then return
    if (!_isFirmware)
    {
        Log_Info(_logger, "UpdateOTA startUpdate: Update completed successfully (not firmware)");
        return UpdateOTAError::SUCCESS;
    }

    // Change the boot partition to the new partition
    err = changeBootPartition();
    if (err != UpdateOTAError::SUCCESS)
    {
        Log_Error(_logger, "UpdateOTA startUpdate error: Failed to change boot partition, ErrorCode=%d", err);
        return err;
    }

    // Restart the ESP
    ESP.restart();

    // Never reached
    return UpdateOTAError::SUCCESS;
}

UpdateOTAError UpdateOTA::getVersionNumber(const char *uRL, char *buffer, uint8_t bufferSize)
{
    Log_Verbose(_logger, "UpdateOTA getVersionNumber: URL='%s', BufferSize=%u", uRL, bufferSize);

    // Check if the device is connected to the internet
    if (WiFi.status() != WL_CONNECTED)
    {
        Log_Error(_logger, "UpdateOTA getVersionNumber error: No internet connection");
        return UpdateOTAError::NO_INTERNET;
    }

    // Set member variables based on input parameters
    _uRL = uRL;
    _wifiClientSecure = new WiFiClientSecure();
    _httpClient = new HTTPClient();
    UpdateOTAError err = UpdateOTAError::SUCCESS;

    // Process the GET request
    err = processGetRequest();
    if (err != UpdateOTAError::SUCCESS)
    {
        Log_Error(_logger, "UpdateOTA getVersionNumber error: Failed to process GET request, ErrorCode=%d", err);
        return err;
    }

    // Check if there is enough space for the firmware
    if (_httpClient->getSize() > bufferSize)
    {
        Log_Error(_logger, "UpdateOTA getVersionNumber error: Insufficient space for update");
        return UpdateOTAError::NO_ENOUGH_SPACE;
    }

    // Read bytes directly into the buffer and null-terminate it
    _wifiClientSecure->readBytes(buffer, _httpClient->getSize());
    buffer[_httpClient->getSize()] = '\0'; // Null-terminate the string

    Log_Debug(_logger, "UpdateOTA getVersionNumber: Version retrieved successfully");
    return UpdateOTAError::SUCCESS;
}

void UpdateOTA::errorToString(UpdateOTAError error, char *buffer, uint8_t bufferSize)
{
    if (buffer == nullptr || bufferSize < 50)
    {
        Log_Error(_logger, "Invalid error string buffer");
        return;
    }
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

    Log_Verbose(_logger, "UpdateOTA processGetRequest: HTTP Code=%d", _httpCode);

    switch (_httpCode)
    {
    case HTTP_CODE_OK:
        Log_Verbose(_logger, "UpdateOTA processGetRequest: Success");
        return UpdateOTAError::SUCCESS;
    case HTTP_CODE_NOT_FOUND:
        Log_Error(_logger, "UpdateOTA processGetRequest error: Page not found");
        return UpdateOTAError::PAGE_NOT_FOUND;
    case HTTP_CODE_UNAUTHORIZED:
        Log_Error(_logger, "UpdateOTA processGetRequest error: Unauthorized access");
        return UpdateOTAError::UNAUTHORIZED;
    case HTTP_CODE_BAD_REQUEST:
        Log_Error(_logger, "UpdateOTA processGetRequest error: Bad request received");
        return UpdateOTAError::BAD_REQUEST;
    default:
        Log_Error(_logger, "UpdateOTA processGetRequest error: Unknown HTTP Code=%d", _httpCode);
        return UpdateOTAError::UNKNOWN;
    }
}

UpdateOTAError UpdateOTA::updateFirmware()
{
    Log_Verbose(_logger, "Updating firmware");
    // Update the firmware
    if (_relayModule != nullptr)
        _relayModule->setState(true);

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

    if (_relayModule != nullptr)
        _relayModule->setState(false);

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
    {
        Log_Error(_logger, "UpdateOTA changeBootPartition error: Failed to set boot partition");
        return UpdateOTAError::PARTITION_NOT_BOOTABLE;
    }

    Log_Verbose(_logger, "UpdateOTA changeBootPartition: Boot partition changed successfully");
    return UpdateOTAError::SUCCESS;
}

UpdateOTAError UpdateOTA::selectPartition()
{
    // Select the partition for update
    if (_isFirmware)
    {
        _newPartition = esp_ota_get_next_update_partition(nullptr);
    }
    else
    {
        _newPartition = esp_partition_find_first(
            ESP_PARTITION_TYPE_DATA,
            ESP_PARTITION_SUBTYPE_DATA_SPIFFS,
            nullptr);
    }

    if (_newPartition == nullptr)
    {
        Log_Error(_logger, "UpdateOTA selectPartition error: No partition available for update");
        return UpdateOTAError::NO_PARTITION_AVAILABLE;
    }

    Log_Verbose(_logger, "UpdateOTA selectPartition: Partition selected successfully");
    return UpdateOTAError::SUCCESS;
}

void UpdateOTA::printProgress(size_t written, size_t total)
{
    // Print the progress
    float progress = (float)written / (float)total * 100;
    Log_Debug(_logger, "UpdateOTA printProgress: Progress=%.2f%%", progress);
}

void UpdateOTA::toggleLed()
{
    // Toggle the LED
    if (_relayModule != nullptr)
        _relayModule->toggle();
}
