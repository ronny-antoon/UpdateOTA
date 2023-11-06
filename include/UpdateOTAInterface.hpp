#ifndef UPDATE_OTA_INTERFACE_HPP
#define UPDATE_OTA_INTERFACE_HPP

/**
 * @file UpdateOTAInterface.hpp
 * @brief Interface class for performing updates using OTA (Over The Air) on ESP32 devices.
 * @author Ronny Antoon
 */

/**
 * @brief Enum representing different error states for the update process.
 */
enum class UPDATE_OTA_ERROR
{
    OK,                           ///< Update completed successfully.
    NO_PARTITION_AVAILABLE,       ///< No partition available for update.
    CHANGE_BOOT_PARTITION_FAILED, ///< Failed to change the boot partition.
    PARTITION_WRITE_FAILED,       ///< Failed to write to the partition.
    READ_FAILED,                  ///< Failed to read from the update data.
    NO_ENOUGH_SPACE,              ///< Not enough space for the update.
    INVALID_ARGUMENT,             ///< Invalid argument.
    NO_NEW_VERSION,               ///< No new version available.
    UNKNOWN,                      ///< An unknown error occurred during the update.
};

/**
 * @brief Interface class for performing updates using OTA (Over The Air) on ESP32 devices.
 *
 * This class defines an interface for performing updates using OTA (Over The Air) on ESP32 devices.
 */
class UpdateOTAInterface
{
public:
    /**
     * @brief Virtual destructor for the UpdateOTAInterface class.
     */
    virtual ~UpdateOTAInterface() = default;

    /**
     * @brief Set a Certificate Authority certificate.
     *
     * @param cACertificate The Certificate Authority certificate.
     * @return UPDATE_OTA_ERROR An error code indicating the result of the update process:
     *          - UPDATE_OTA_ERROR::OK: Update completed successfully.
     *          - UPDATE_OTA_ERROR::INVALID_ARGUMENT: Invalid argument.
     *          - UPDATE_OTA_ERROR::UNKNOWN: An unknown error occurred during the update.
     */
    virtual UPDATE_OTA_ERROR setCACertificate(const char *cACertificate) = 0;

    /**
     * @brief Update firmware.
     *
     * @param urlFirmware The URL of the firmware file.
     * @param urlVersion The URL of the version file.
     * @param currentVersion The current version of the firmware.
     * @return UPDATE_OTA_ERROR An error code indicating the result of the update process:
     *          - UPDATE_OTA_ERROR::OK: Update completed successfully.
     *          - UPDATE_OTA_ERROR::INVALID_ARGUMENT: Invalid argument.
     *          - UPDATE_OTA_ERROR::NO_NEW_VERSION: No new version available.
     *          - UPDATE_OTA_ERROR::UNKNOWN: An unknown error occurred during the update.
     */
    virtual UPDATE_OTA_ERROR updateFirmaware(const char *urlFirmware, const char *urlVersion = "", const char *currentVersion = "") = 0;

    /**
     * @brief Update SPIFFS.
     *
     * @param url The URL of the SPIFFS file.
     * @param urlVersion The URL of the version file.
     * @param currentVersion The current version of the SPIFFS.
     * @return UPDATE_OTA_ERROR An error code indicating the result of the update process:
     *          - UPDATE_OTA_ERROR::OK: Update completed successfully.
     *          - UPDATE_OTA_ERROR::INVALID_ARGUMENT: Invalid argument.
     *          - UPDATE_OTA_ERROR::NO_NEW_VERSION: No new version available.
     *          - UPDATE_OTA_ERROR::UNKNOWN: An unknown error occurred during the update.
     */
    virtual UPDATE_OTA_ERROR updateSPIFFS(const char *url, const char *urlVersion = "", const char *currentVersion = "") = 0;

    /**
     * @brief Get the server version.
     *
     * @param url The URL of the version file.
     * @param versionDist The version of the file on the server.
     * @param maxLength The maximum length of the version string.
     * @return UPDATE_OTA_ERROR An error code indicating the result of the update process:
     *          - UPDATE_OTA_ERROR::OK: Update completed successfully.
     *          - UPDATE_OTA_ERROR::INVALID_ARGUMENT: Invalid argument.
     *          - UPDATE_OTA_ERROR::NO_NEW_VERSION: No new version available.
     *          - UPDATE_OTA_ERROR::UNKNOWN: An unknown error occurred during the update.
     */
    virtual UPDATE_OTA_ERROR getServerVersion(const char *url, char *versionDist, int maxLength) = 0;
};

#endif // UPDATE_OTA_INTERFACE_HPP