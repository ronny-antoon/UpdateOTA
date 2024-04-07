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

    RelayModuleInterface *_relayModule = nullptr;   ///< RelayModuleInterface instance for controlling an LED during the update
    MultiPrinterLoggerInterface *_logger = nullptr; ///< Logger for logging messages
    WiFiClientSecure *_wifiClientSecure = nullptr;  ///< WiFiClientSecure instance for secure communication
    HTTPClient *_httpClient = nullptr;              ///< HTTPClient instance for handling HTTP requests
    uint16_t _httpCode = 0;                         ///< HTTP response code
    const char *_uRL;                               ///< URL for the update
    char _buffer[BLOCK_SIZE_P];                     ///< Buffer for reading/writing data blocks
    const esp_partition_t *_newPartition;           ///< Pointer to the new partition for firmware update
    bool _isFirmware = false;                       ///< Flag indicating whether the update is for firmware
    char ca_cert[1600] =                            ///< Certificate data for secure communication
    "-----BEGIN CERTIFICATE-----\n"
"MIICjzCCAhWgAwIBAgIQXIuZxVqUxdJxVt7NiYDMJjAKBggqhkjOPQQDAzCBiDEL\n"
"MAkGA1UEBhMCVVMxEzARBgNVBAgTCk5ldyBKZXJzZXkxFDASBgNVBAcTC0plcnNl\n"
"eSBDaXR5MR4wHAYDVQQKExVUaGUgVVNFUlRSVVNUIE5ldHdvcmsxLjAsBgNVBAMT\n"
"JVVTRVJUcnVzdCBFQ0MgQ2VydGlmaWNhdGlvbiBBdXRob3JpdHkwHhcNMTAwMjAx\n"
"MDAwMDAwWhcNMzgwMTE4MjM1OTU5WjCBiDELMAkGA1UEBhMCVVMxEzARBgNVBAgT\n"
"Ck5ldyBKZXJzZXkxFDASBgNVBAcTC0plcnNleSBDaXR5MR4wHAYDVQQKExVUaGUg\n"
"VVNFUlRSVVNUIE5ldHdvcmsxLjAsBgNVBAMTJVVTRVJUcnVzdCBFQ0MgQ2VydGlm\n"
"aWNhdGlvbiBBdXRob3JpdHkwdjAQBgcqhkjOPQIBBgUrgQQAIgNiAAQarFRaqflo\n"
"I+d61SRvU8Za2EurxtW20eZzca7dnNYMYf3boIkDuAUU7FfO7l0/4iGzzvfUinng\n"
"o4N+LZfQYcTxmdwlkWOrfzCjtHDix6EznPO/LlxTsV+zfTJ/ijTjeXmjQjBAMB0G\n"
"A1UdDgQWBBQ64QmG1M8ZwpZ2dEl23OA1xmNjmjAOBgNVHQ8BAf8EBAMCAQYwDwYD\n"
"VR0TAQH/BAUwAwEB/zAKBggqhkjOPQQDAwNoADBlAjA2Z6EWCNzklwBBHU6+4WMB\n"
"zzuqQhFkoJ2UOQIReVx7Hfpkue4WQrO/isIJxOzksU0CMQDpKmFHjFJKS04YcPbW\n"
"RNZu9YO6bVi9JNlWSOrvxKJGgYhqOkbRqZtNyWHa0V1Xahg=\n"
"-----END CERTIFICATE-----\n";

        // "-----BEGIN CERTIFICATE-----\n"
        // "MIIDjjCCAnagAwIBAgIQAzrx5qcRqaC7KGSxHQn65TANBgkqhkiG9w0BAQsFADBh\n"
        // "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"
        // "d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBH\n"
        // "MjAeFw0xMzA4MDExMjAwMDBaFw0zODAxMTUxMjAwMDBaMGExCzAJBgNVBAYTAlVT\n"
        // "MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n"
        // "b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IEcyMIIBIjANBgkqhkiG\n"
        // "9w0BAQEFAAOCAQ8AMIIBCgKCAQEAuzfNNNx7a8myaJCtSnX/RrohCgiN9RlUyfuI\n"
        // "2/Ou8jqJkTx65qsGGmvPrC3oXgkkRLpimn7Wo6h+4FR1IAWsULecYxpsMNzaHxmx\n"
        // "1x7e/dfgy5SDN67sH0NO3Xss0r0upS/kqbitOtSZpLYl6ZtrAGCSYP9PIUkY92eQ\n"
        // "q2EGnI/yuum06ZIya7XzV+hdG82MHauVBJVJ8zUtluNJbd134/tJS7SsVQepj5Wz\n"
        // "tCO7TG1F8PapspUwtP1MVYwnSlcUfIKdzXOS0xZKBgyMUNGPHgm+F6HmIcr9g+UQ\n"
        // "vIOlCsRnKPZzFBQ9RnbDhxSJITRNrw9FDKZJobq7nMWxM4MphQIDAQABo0IwQDAP\n"
        // "BgNVHRMBAf8EBTADAQH/MA4GA1UdDwEB/wQEAwIBhjAdBgNVHQ4EFgQUTiJUIBiV\n"
        // "5uNu5g/6+rkS7QYXjzkwDQYJKoZIhvcNAQELBQADggEBAGBnKJRvDkhj6zHd6mcY\n"
        // "1Yl9PMWLSn/pvtsrF9+wX3N3KjITOYFnQoQj8kVnNeyIv/iPsGEMNKSuIEyExtv4\n"
        // "NeF22d+mQrvHRAiGfzZ0JFrabA0UWTW98kndth/Jsw1HKj2ZL7tcu7XUIOGZX1NG\n"
        // "Fdtom/DzMNU+MeKNhJ7jitralj41E6Vf8PlwUHBHQRFXGU7Aj64GxJUTFy8bJZ91\n"
        // "8rGOmaFvE7FBcf6IKshPECBV1/MUReXgRPTqh5Uykw7+U0b6LJ3/iyK5S9kJRaTe\n"
        // "pLiaWN0bfVKfjllDiIGknibVb63dDcY3fe0Dkhvld1927jyNxF1WW6LZZm6zNTfl\n"
        // "MrY=\n"
        // "-----END CERTIFICATE-----\n";
};

#endif // UPDATE
