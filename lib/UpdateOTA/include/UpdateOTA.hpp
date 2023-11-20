#ifndef UPDATE_OTA_HPP
#define UPDATE_OTA_HPP

#include <MultiPrinterLoggerInterface.hpp> // MultiPrinterLoggerInterface
#include <RelayModuleInterface.hpp>        // RelayModuleInterface
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <esp_ota_ops.h>
#include <esp_partition.h>

#include "UpdateOTAInterface.hpp"

/**
 * @brief Class for handling Over-The-Air (OTA) updates
 */
class UpdateOTA : public UpdateOTAInterface
{
public:
    /**
     * @brief Constructor
     */
    UpdateOTA(MultiPrinterLoggerInterface *logger = nullptr, RelayModuleInterface *relayModule = nullptr);

    /**
     * @brief Destructor
     */
    ~UpdateOTA();

    /**
     * @brief Start the OTA update process with the specified URL, update type, and pin status
     * @param uRL The URL of the update
     * @param isFirmware Flag indicating whether the update is for firmware
     * @param pinStatus The pin status for controlling an LED during the update
     * @return UpdateOTAError indicating the success or failure of the OTA update process, Options:-
     *      UpdateOTAError::SUCCESS                 - If the OTA update process completed successfully
     *      UpdateOTAError::NO_INTERNET             - If there is no internet connection
     *      UpdateOTAError::PAGE_NOT_FOUND          - If the page is not found
     *      UpdateOTAError::UNAUTHORIZED            - If there is an unauthorized access error
     *      UpdateOTAError::BAD_REQUEST             - If there is a bad request error
     *      UpdateOTAError::NO_ENOUGH_SPACE         - If there is insufficient space for the update
     *      UpdateOTAError::NO_PARTITION_AVAILABLE  - If there is no available partition for update
     *      UpdateOTAError::UPDATE_PROGRESS_ERROR   - If there is an error in the update progress
     *      UpdateOTAError::PARTITION_NOT_BOOTABLE  - If the selected partition is not bootable
     */
    UpdateOTAError startUpdate(const char *uRL, bool isFirmware) override;

    /**
     * @brief Get the version number from the specified URL and store it in the provided buffer
     * @param uRL The URL to get the version number from
     * @param buffer Buffer to store the version number
     * @param bufferSize Size of the buffer
     * @return UpdateOTAError indicating the success or failure of the operation, Options:-
     *      UpdateOTAError::SUCCESS         - If the version number was retrieved successfully
     *      UpdateOTAError::NO_INTERNET     - If there is no internet connection
     *      UpdateOTAError::PAGE_NOT_FOUND  - If the page is not found
     *      UpdateOTAError::UNAUTHORIZED    - If there is an unauthorized access error
     *      UpdateOTAError::BAD_REQUEST     - If there is a bad request error
     *      UpdateOTAError::NO_ENOUGH_SPACE - If there is insufficient space for the update
     *      UpdateOTAError::UNKNOWN         - If there is an unknown error
     */
    UpdateOTAError getVersionNumber(const char *uRL, char *buffer, uint8_t bufferSize) override;

    /**
     * @brief Convert an update OTA error to a human-readable string and store it in the provided buffer
     * @param error The update OTA error
     * @param buffer Buffer to store the error message
     * @param bufferSize Size of the buffer
     */
    void errorToString(UpdateOTAError error, char *buffer, uint8_t bufferSize) override;

private:
    /**
     * @brief Process a GET request for the update version
     * @return UpdateOTAError indicating the success or failure of the request, Options:-
     *      UpdateOTAError::SUCCESS         - If the request was successful
     *      UpdateOTAError::PAGE_NOT_FOUND  - If the page is not found
     *      UpdateOTAError::UNAUTHORIZED    - If there is an unauthorized access error
     *      UpdateOTAError::BAD_REQUEST     - If there is a bad request error
     *      UpdateOTAError::UNKNOWN         - If there is an unknown error
     */
    UpdateOTAError processGetRequest();

    /**
     * @brief Update the firmware
     * @return UpdateOTAError indicating the success or failure of the firmware update, Options:-
     *      UpdateOTAError::SUCCESS                 - If the firmware update was successful
     *      UpdateOTAError::UPDATE_PROGRESS_ERROR   - If there is an error in the update progress
     */
    UpdateOTAError updateFirmware();

    /**
     * @brief Reset the buffer to zero
     */
    void resetBuffer();

    /**
     * @brief Reset the specified range of the partition
     * @param offset Offset of the partition
     * @param length Length of the partition
     */
    void resetPartitionRange(size_t offset, size_t length);

    /**
     * @brief Write the block buffer to the partition
     * @param offset Offset to write to
     * @param length Length of the block buffer
     */
    void writeBlockBufferToPartition(size_t offset, size_t length);

    /**
     * @brief Read a block from the client to the buffer
     * @param offset Offset to read from
     * @param length Length of the block to read
     * @return Size of the block read
     */
    size_t readBlockFromClientToBuffer(size_t offset, size_t length);

    /**
     * @brief Change the boot partition to the new partition
     * @return UpdateOTAError indicating the success or failure of the operation, Options:-
     *      UpdateOTAError::SUCCESS                 - If the boot partition was changed successfully
     *      UpdateOTAError::PARTITION_NOT_BOOTABLE  - If the selected partition is not bootable
     */
    UpdateOTAError changeBootPartition();

    /**
     * @brief Select the partition for update
     * @return UpdateOTAError indicating the success or failure of the operation, Options:-
     *      UpdateOTAError::SUCCESS                 - If the partition was selected successfully
     *      UpdateOTAError::NO_PARTITION_AVAILABLE  - If there is no available partition for update
     */
    UpdateOTAError selectPartition();

    /**
     * @brief Print the update progress
     * @param written Number of bytes written
     * @param total Total number of bytes
     */
    void printProgress(size_t written, size_t total);

    /**
     * @brief Toggle the LED if pinStatus is not zero
     */
    void toggleLed();

    RelayModuleInterface *_relayModule;   ///< RelayModuleInterface instance for controlling an LED during the update
    MultiPrinterLoggerInterface *_logger; ///< Logger for logging messages
    WiFiClientSecure *_wifiClientSecure;  ///< WiFiClientSecure instance for secure communication
    HTTPClient *_httpClient;              ///< HTTPClient instance for handling HTTP requests
    uint16_t _httpCode;                   ///< HTTP response code
    const char *_uRL;                     ///< URL for the update
    char _buffer[BLOCK_SIZE_P];           ///< Buffer for reading/writing data blocks
    const esp_partition_t *_newPartition; ///< Pointer to the new partition for firmware update
    bool _isFirmware;                     ///< Flag indicating whether the update is for firmware
    char ca_cert[1400] =                  ///< Certificate data for secure communication
        "-----BEGIN CERTIFICATE-----\n"
        // (Certificate data goes here)
        "MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh\n"
        "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"
        "d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n"
        "QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT\n"
        "MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n"
        "b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG\n"
        "9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB\n"
        "CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97\n"
        "nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt\n"
        "43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P\n"
        "T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4\n"
        "gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO\n"
        "BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR\n"
        "TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw\n"
        "DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr\n"
        "hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg\n"
        "06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF\n"
        "PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls\n"
        "YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk\n"
        "CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=\n"
        "-----END CERTIFICATE-----\n";
};

#endif // UPDATE
