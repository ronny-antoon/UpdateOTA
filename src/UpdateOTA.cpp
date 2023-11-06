/**
 * @file UpdateOTA.cpp
 * @author Ronny Antoon
 */
#include <API_MultiPrinterLogger.hpp> // Log_Debug, Log_Error

#include "UpdateOTA.hpp"
#include "DownloaderInterface.hpp"
#include "UpdaterInterface.hpp"

UpdateOTA::UpdateOTA(DownloaderInterface *downloader, UpdaterInterface *updater)
{
    _downloader = downloader; // Dependency Injection
    _updater = updater;       // Dependency Injection
}

UPDATE_OTA_ERROR UpdateOTA::setCACertificate(const char *cACertificate)
{
    // Check if the CA certificate is not too long or null
    if (cACertificate == nullptr || strlen(cACertificate) > CA_MAX_LENGTH)
    {
        Log_Error(TAG, "CA certificate is too long");
        return UPDATE_OTA_ERROR::UNKNOWN;
    }

    // Copy the CA certificate to the private variable, And check if the CA certificate is empty
    size_t caLength = strlen(cACertificate) < CA_MAX_LENGTH ? strlen(cACertificate) : CA_MAX_LENGTH;
    strncpy(_cACertificate, cACertificate, caLength);
    _cACertificate[caLength] = '\0';
    if (strlen(_cACertificate) == 0)
    {
        Log_Error(TAG, "CA Certificate is empty");
        return UPDATE_OTA_ERROR::INVALID_ARGUMENT;
    }

    Log_Debug(TAG, "CA Certificate Registered.");

    return UPDATE_OTA_ERROR::OK;
}

UPDATE_OTA_ERROR UpdateOTA::updateFirmaware(const char *urlFirmware, const char *urlVersion, const char *currentVersion)
{
    // Check if the URL is not too long or null
    if (urlFirmware == nullptr || strlen(urlFirmware) == 0)
    {
        Log_Error(TAG, "URL is null or empty");
        return UPDATE_OTA_ERROR::INVALID_ARGUMENT;
    }

    // Check if the CA certificate provided, And set it to the downloader
    if (strlen(_cACertificate) != 0)
    {
        if (_downloader->setCA(_cACertificate) != DOWNLOADER_ERROR::OK)
        {
            Log_Error(TAG, "Error setting CA Certificate");
            return UPDATE_OTA_ERROR::UNKNOWN;
        }
    }

    // Check if the current version provided, then check if there is a newer version
    if (currentVersion != nullptr && strlen(currentVersion) != 0)
    {
        // Set the URL for the version file
        if (_downloader->setURLForVersion(urlVersion) != DOWNLOADER_ERROR::OK)
        {
            Log_Error(TAG, "Error setting URL for version file");
            return UPDATE_OTA_ERROR::UNKNOWN;
        }

        // Get the version from the server
        char versionDist[20];
        if (_downloader->getVersion(versionDist, 20) != DOWNLOADER_ERROR::OK)
        {
            Log_Error(TAG, "Error getting server version");
            return UPDATE_OTA_ERROR::UNKNOWN;
        }

        // Check if there is a newer version
        if (newerVersion(currentVersion, versionDist))
        {
            Log_Debug(TAG, "Newer version found");
        }
        else
        {
            Log_Debug(TAG, "No newer version found");
            return UPDATE_OTA_ERROR::NO_NEW_VERSION;
        }
    }

    // Set the URL for the bin file
    if (_downloader->setURLForBin(urlFirmware) != DOWNLOADER_ERROR::OK)
    {
        Log_Error(TAG, "Error setting URL for bin file");
        return UPDATE_OTA_ERROR::UNKNOWN;
    }

    // Download the bin file
    if (_downloader->download() != DOWNLOADER_ERROR::OK)
    {
        Log_Error(TAG, "Error downloading bin file");
        return UPDATE_OTA_ERROR::UNKNOWN;
    }

    // Start the update process
    if (_updater->startUpdate(_downloader->getStreamPtr(), _downloader->getStreamSize(), UPDATER_TYPE::FIRMWARE) != UPDATER_ERROR::OK)
    {
        Log_Error(TAG, "Error updating firmware");
        return UPDATE_OTA_ERROR::UNKNOWN;
    }

    return UPDATE_OTA_ERROR::OK;
}

UPDATE_OTA_ERROR UpdateOTA::updateSPIFFS(const char *url, const char *urlVersion, const char *currentVersion)
{
    // Check if the URL is not too long or null
    if (url == nullptr || strlen(url) == 0)
    {
        Log_Error(TAG, "URL is null or empty");
        return UPDATE_OTA_ERROR::INVALID_ARGUMENT;
    }

    // Check if the CA certificate provided, And set it to the downloader
    if (strlen(_cACertificate) != 0)
    {
        if (_downloader->setCA(_cACertificate) != DOWNLOADER_ERROR::OK)
        {
            Log_Error(TAG, "Error setting CA Certificate");
            return UPDATE_OTA_ERROR::UNKNOWN;
        }
    }

    // Check if the current version provided, then check if there is a newer version
    if (currentVersion != nullptr && strlen(currentVersion) != 0)
    {
        // Set the URL for the version file
        if (_downloader->setURLForVersion(urlVersion) != DOWNLOADER_ERROR::OK)
        {
            Log_Error(TAG, "Error setting URL for version file");
            return UPDATE_OTA_ERROR::UNKNOWN;
        }

        // Get the version from the server
        char versionDist[20];
        if (_downloader->getVersion(versionDist, 20) != DOWNLOADER_ERROR::OK)
        {
            Log_Error(TAG, "Error getting server version");
            return UPDATE_OTA_ERROR::UNKNOWN;
        }

        // Check if there is a newer version
        if (newerVersion(currentVersion, versionDist))
        {
            Log_Debug(TAG, "Newer version found");
        }
        else
        {
            Log_Debug(TAG, "No newer version found");
            return UPDATE_OTA_ERROR::UNKNOWN;
        }
    }

    // Set the URL for the bin file
    if (_downloader->setURLForBin(url) != DOWNLOADER_ERROR::OK)
    {
        Log_Error(TAG, "Error setting URL for bin file");
        return UPDATE_OTA_ERROR::UNKNOWN;
    }

    // Download the bin file
    if (_downloader->download() != DOWNLOADER_ERROR::OK)
    {
        Log_Error(TAG, "Error downloading bin file");
        return UPDATE_OTA_ERROR::UNKNOWN;
    }

    // Start the update process
    if (_updater->startUpdate(_downloader->getStreamPtr(), _downloader->getStreamSize(), UPDATER_TYPE::SPIFFS) != UPDATER_ERROR::OK)
    {
        Log_Error(TAG, "Error updating SPIFFS");
        return UPDATE_OTA_ERROR::UNKNOWN;
    }

    return UPDATE_OTA_ERROR::OK;
}

UPDATE_OTA_ERROR UpdateOTA::getServerVersion(const char *url, char *versionDist, int maxLength)
{
    // Check if the URL is not too long or null
    if (url == nullptr || strlen(url) == 0)
    {
        Log_Error(TAG, "URL is null or empty");
        return UPDATE_OTA_ERROR::INVALID_ARGUMENT;
    }

    // Check if the CA certificate provided, And set it to the downloader
    if (strlen(_cACertificate) != 0)
    {
        if (_downloader->setCA(_cACertificate) != DOWNLOADER_ERROR::OK)
        {
            Log_Error(TAG, "Error setting CA Certificate");
            return UPDATE_OTA_ERROR::UNKNOWN;
        }
    }

    // Set the URL for the version file
    if (_downloader->setURLForVersion(url) != DOWNLOADER_ERROR::OK)
    {
        Log_Error(TAG, "Error setting URL for version file");
        return UPDATE_OTA_ERROR::UNKNOWN;
    }

    // Get the version from the server
    if (_downloader->getVersion(versionDist, maxLength) != DOWNLOADER_ERROR::OK)
    {
        Log_Error(TAG, "Error getting server version");
        return UPDATE_OTA_ERROR::NO_NEW_VERSION;
    }

    return UPDATE_OTA_ERROR::OK;
}

bool UpdateOTA::newerVersion(const char *_currentVersion, const char *_serverVersion)
{
    bool result = false;

    char majorCurrentVersion[32];
    char minorCurrentVersion[32];
    char patchCurrentVersion[32];

    char majorServerVersion[32];
    char minorServerVersion[32];
    char patchServerVersion[32];

    int countrDotsCurrentVersion = 0;
    int majorCurrentVersionLength = 0;
    int minorCurrentVersionLength = 0;
    int patchCurrentVersionLength = 0;

    int countrDotsServerVersion = 0;
    int majorServerVersionLength = 0;
    int minorServerVersionLength = 0;
    int patchServerVersionLength = 0;

    // Split the current version into major, minor and patch
    for (int i = 0; i < 32; i++)
    {
        if (_currentVersion[i] == '.')
        {
            countrDotsCurrentVersion++;
            continue;
        }

        if (countrDotsCurrentVersion == 0)
        {
            majorCurrentVersion[majorCurrentVersionLength] = _currentVersion[i];
            majorCurrentVersionLength++;
        }
        else if (countrDotsCurrentVersion == 1)
        {
            minorCurrentVersion[minorCurrentVersionLength] = _currentVersion[i];
            minorCurrentVersionLength++;
        }
        else if (countrDotsCurrentVersion == 2)
        {
            patchCurrentVersion[patchCurrentVersionLength] = _currentVersion[i];
            patchCurrentVersionLength++;
        }
    }
    majorCurrentVersion[majorCurrentVersionLength] = '\0';
    minorCurrentVersion[minorCurrentVersionLength] = '\0';
    patchCurrentVersion[patchCurrentVersionLength] = '\0';

    // Split the server version into major, minor and patch
    for (int i = 0; i < 32; i++)
    {
        if (_serverVersion[i] == '.')
        {
            countrDotsServerVersion++;
            continue;
        }

        if (countrDotsServerVersion == 0)
        {
            majorServerVersion[majorServerVersionLength] = _serverVersion[i];
            majorServerVersionLength++;
        }
        else if (countrDotsServerVersion == 1)
        {
            minorServerVersion[minorServerVersionLength] = _serverVersion[i];
            minorServerVersionLength++;
        }
        else if (countrDotsServerVersion == 2)
        {
            patchServerVersion[patchServerVersionLength] = _serverVersion[i];
            patchServerVersionLength++;
        }
    }

    majorServerVersion[majorServerVersionLength] = '\0';
    minorServerVersion[minorServerVersionLength] = '\0';
    patchServerVersion[patchServerVersionLength] = '\0';

    // Compare the versions
    if (atoi(majorCurrentVersion) < atoi(majorServerVersion))
    {
        result = true;
    }
    else if (atoi(majorCurrentVersion) == atoi(majorServerVersion))
    {
        if (atoi(minorCurrentVersion) < atoi(minorServerVersion))
        {
            result = true;
        }
        else if (atoi(minorCurrentVersion) == atoi(minorServerVersion))
        {
            if (atoi(patchCurrentVersion) < atoi(patchServerVersion))
            {
                result = true;
            }
        }
    }

    return result;
}
