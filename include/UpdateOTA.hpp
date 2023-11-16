#ifndef UPDATE_OTA_HPP
#define UPDATE_OTA_HPP

/**
 * @file UpdateOTA.hpp
 * @brief Class for donwload and perform updates on ESP32 devices.
 * @author Ronny Antoon
 */

#include <MultiPrinterLoggerInterface.hpp>

#include "DownloaderInterface.hpp"
#include "UpdaterInterface.hpp"
#include "UpdateOTAInterface.hpp"

/**
 * @brief UpdateOTA class for donwload and perform updates on ESP32 devices.
 *
 * This class provides methods for donwload and perform updates on ESP32 devices.
 */
class UpdateOTA : public UpdateOTAInterface
{
public:
    /**
     * @brief Constructor for the UpdateOTA class.
     *
     * @param downloader A pointer to a DownloaderInterface object. (Dependency Injection)
     * @param updater A pointer to a UpdaterInterface object. (Dependency Injection)
     */
    UpdateOTA(DownloaderInterface *downloader, UpdaterInterface *updater, MultiPrinterLoggerInterface *logger);

    /**
     * @brief Destructor for the UpdateOTA class.
     */
    ~UpdateOTA() = default;

    /**
     * @brief Set a Certificate Authority certificate.
     *
     * @param cACertificate The Certificate Authority certificate.
     * @return UPDATE_OTA_ERROR An error code indicating the result of the update process:
     *          - UPDATE_OTA_ERROR::OK: Update completed successfully.
     *          - UPDATE_OTA_ERROR::INVALID_ARGUMENT: Invalid argument.
     *          - UPDATE_OTA_ERROR::UNKNOWN: An unknown error occurred during the update.
     */
    UPDATE_OTA_ERROR setCACertificate(const char *cACertificate) override;

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
    UPDATE_OTA_ERROR updateFirmaware(const char *urlFirmware, const char *urlVersion = "", const char *currentVersion = "") override;

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
    UPDATE_OTA_ERROR updateSPIFFS(const char *url, const char *urlVersion = "", const char *currentVersion = "") override;

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
    UPDATE_OTA_ERROR getServerVersion(const char *url, char *versionDist, int maxLength) override;

private:
    DownloaderInterface *_downloader;       ///< A pointer to a DownloaderInterface object.
    UpdaterInterface *_updater;             ///< A pointer to a UpdaterInterface object.
    char _cACertificate[CA_MAX_LENGTH + 1]; ///< The CA certificate.

    /**
     * @brief Check if the server version is newer than the current version.
     *
     * @param _currentVersion The current version.
     * @param _serverVersion The server version.
     * @return true The server version is newer than the current version.
     * @return false The server version is not newer than the current version.
     */
    bool newerVersion(const char *_currentVersion, const char *_serverVersion);

    MultiPrinterLoggerInterface *_logger;
};

#endif // UPDATE_OTA_HPP