#include <string.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Stream.h>
#include <API_MultiPrinterLogger.hpp>

#include "Downloader.hpp"

Downloader::Downloader()
{
    // Initialize default values for the Downloader class.
    _timeOut = 8000;              // Default timeout value in milliseconds.
    _uRLForBinFile[0] = '\0';     // Initialize URL for binary file to an empty string.
    _uRLForVersionFile[0] = '\0'; // Initialize URL for version file to an empty string.
    _cACertificate[0] = '\0';     // Initialize CA certificate to an empty string.
    _wifiClient = nullptr;        // Initialize WiFi client pointer to nullptr.
    _httpClient = nullptr;        // Initialize HTTP client pointer to nullptr.
    _streamSize = 0;              // Initialize stream size to zero.
}

Downloader::~Downloader()
{
    // Destructor for cleaning up resources.
    if (_httpClient != nullptr)
    {
        _httpClient->end(); // Close and release the HTTP client.
        delete _httpClient; // Deallocate memory for the HTTP client.
    }

    if (_wifiClient != nullptr)
    {
        _wifiClient->flush(); // Flush the WiFi client's buffer.
        _wifiClient->stop();  // Stop the WiFi client.
        if (strlen(_cACertificate) > 0)
            delete ((WiFiClientSecure *)_wifiClient); // Deallocate memory for secure WiFi client.
        else
            delete _wifiClient; // Deallocate memory for regular WiFi client.
    }
}

DOWNLOADER_ERROR Downloader::download()
{
    int downloadTimer = millis();       // Record the start time of the download.
    Log_Debug(TAG, "Download started"); // Log a debug message.

    // Check if the URL for the binary file is empty.
    if (strlen(_uRLForBinFile) == 0)
    {
        Log_Error(TAG, "URL for bin file is empty"); // Log an error message.
        return DOWNLOADER_ERROR::NO_URL_PROVIDED;    // Return an error indicating no URL provided.
    }

    CreateWifiClient(); // Create a WiFi client based on the CA certificate, if available.

    // Create an HTTP client and initialize it with the URL for the binary file.
    if (CreateHttpClient(_uRLForBinFile) != DOWNLOADER_ERROR::OK)
        return DOWNLOADER_ERROR::UNKNOWN; // Return an unknown error if HTTP client creation fails.

    int httpCode = _httpClient->GET(); // Send an HTTP GET request.
    if (httpCode != HTTP_CODE_OK)
    {
        Log_Error(TAG, "HTTP GET failed, error: %s", _httpClient->errorToString(httpCode).c_str());
        return DOWNLOADER_ERROR::SERVER_ERROR; // Return an unknown error if the HTTP GET request fails.
    }

    _streamSize = _httpClient->getSize(); // Get the size of the downloaded stream.
    if (_streamSize <= 0)
    {
        Log_Error(TAG, "Server returned invalid Content-Length header");
        return DOWNLOADER_ERROR::DOWNLOAD_FAILED; // Return an error indicating a download failure.
    }

    // Log a debug message with the download statistics.
    Log_Debug(TAG, "Download finished, %d bytes, %.2f seconds", _streamSize, (double)(millis() - downloadTimer) / 1000);
    return DOWNLOADER_ERROR::OK; // Return success.
}

DOWNLOADER_ERROR Downloader::getVersion(char *versionDis, int maxLen)
{
    if (versionDis == nullptr)
    {
        Log_Error(TAG, "Version is null");         // Log an error if the version buffer is null.
        return DOWNLOADER_ERROR::INVALID_ARGUMENT; // Return an error indicating an invalid argument.
    }

    if (maxLen <= 0)
    {
        Log_Error(TAG, "Version length is invalid"); // Log an error if the version buffer is null.
        return DOWNLOADER_ERROR::INVALID_ARGUMENT;   // Return an error indicating an invalid argument.
    }

    for (int i = 0; i < maxLen; i++)
        versionDis[i] = '\0'; // Initialize the version buffer with null characters.

    // Check if the URL for the version file is empty.
    if (strlen(_uRLForVersionFile) == 0)
    {
        Log_Error(TAG, "URL for version file is empty"); // Log an error message.
        return DOWNLOADER_ERROR::NO_URL_PROVIDED;        // Return an error indicating no URL provided.
    }

    CreateWifiClient(); // Create a WiFi client based on the CA certificate, if available.

    // Create an HTTP client and initialize it with the URL for the version file.
    if (CreateHttpClient(_uRLForVersionFile) != DOWNLOADER_ERROR::OK)
        return DOWNLOADER_ERROR::UNKNOWN; // Return an unknown error if HTTP client creation fails.

    int httpCode = _httpClient->GET(); // Send an HTTP GET request.
    if (httpCode != HTTP_CODE_OK)
    {
        Log_Error(TAG, "HTTP GET failed, error code: %d - error string: %s", httpCode, _httpClient->errorToString(httpCode).c_str());
        return DOWNLOADER_ERROR::SERVER_ERROR; // Return an unknown error if the HTTP GET request fails.
    }

    _streamSize = _httpClient->getSize(); // Get the size of the downloaded stream.
    if (_streamSize <= 0)
    {
        Log_Error(TAG, "Server returned invalid Content-Length header");
        return DOWNLOADER_ERROR::DOWNLOAD_FAILED; // Return an error indicating a download failure.
    }

    _httpClient->getStreamPtr()->readBytesUntil('\n', versionDis, maxLen); // Read version information from the stream.

    if (strlen(versionDis) == 0)
    {
        Log_Error(TAG, "Version is empty");
        return DOWNLOADER_ERROR::DOWNLOAD_FAILED; // Return an error indicating a download failure.
    }

    // Log the retrieved version information.
    Log_Debug(TAG, "Version: %s", versionDis);

    return DOWNLOADER_ERROR::OK; // Return success.
}

DOWNLOADER_ERROR Downloader::setCA(const char *cACertificate)
{
    if (cACertificate == nullptr || strlen(cACertificate) >= CA_MAX_LENGTH)
    {
        Log_Error(TAG, "CA Certificate is null");  // Log an error if the CA certificate is null or too long.
        return DOWNLOADER_ERROR::INVALID_ARGUMENT; // Return an error indicating an invalid argument.
    }

    size_t caLength = strlen(cACertificate) < CA_MAX_LENGTH ? strlen(cACertificate) : CA_MAX_LENGTH;
    strncpy(_cACertificate, cACertificate, caLength); // Copy and set the CA certificate.

    if (strlen(_cACertificate) == 0)
    {
        Log_Error(TAG, "CA Certificate is empty"); // Log an error if the CA certificate is empty.
        return DOWNLOADER_ERROR::INVALID_ARGUMENT; // Return an error indicating an invalid argument.
    }

    Log_Debug(TAG, "CA Certificate Registered"); // Log a debug message.

    return DOWNLOADER_ERROR::OK; // Return success.
}

DOWNLOADER_ERROR Downloader::setURLForBin(const char *uRLForBinFile)
{
    if (uRLForBinFile == nullptr)
    {
        Log_Error(TAG, "URL for bin file is null"); // Log an error if the URL for the binary file is null.
        return DOWNLOADER_ERROR::INVALID_ARGUMENT;  // Return an error indicating an invalid argument.
    }

    size_t urlLength = strlen(uRLForBinFile) < URL_MAX_LENGTH ? strlen(uRLForBinFile) : URL_MAX_LENGTH;
    strncpy(_uRLForBinFile, uRLForBinFile, urlLength); // Copy and set the URL for the binary file.
    _uRLForBinFile[urlLength] = '\0';                  // Add a null character to the end of the URL.

    if (strlen(_uRLForBinFile) == 0)
    {
        Log_Error(TAG, "URL for bin file is empty"); // Log an error if the URL for the binary file is empty.
        return DOWNLOADER_ERROR::INVALID_ARGUMENT;   // Return an error indicating an invalid argument.
    }

    Log_Debug(TAG, "URL for bin file registered"); // Log a debug message.

    return DOWNLOADER_ERROR::OK; // Return success.
}

DOWNLOADER_ERROR Downloader::setURLForVersion(const char *uRLForVersionFile)
{
    if (uRLForVersionFile == nullptr)
    {
        Log_Error(TAG, "URL for version file is null"); // Log an error if the URL for the version file is null.
        return DOWNLOADER_ERROR::INVALID_ARGUMENT;      // Return an error indicating an invalid argument.
    }

    size_t urlLength = strlen(uRLForVersionFile) < URL_MAX_LENGTH ? strlen(uRLForVersionFile) : URL_MAX_LENGTH;
    strncpy(_uRLForVersionFile, uRLForVersionFile, urlLength); // Copy and set the URL for the version file.
    _uRLForVersionFile[urlLength] = '\0';                      // Add a null character to the end of the URL.

    if (strlen(_uRLForVersionFile) == 0)
    {
        Log_Error(TAG, "URL for version file is empty"); // Log an error if the URL for the version file is empty.
        return DOWNLOADER_ERROR::INVALID_ARGUMENT;       // Return an error indicating an invalid argument.
    }

    Log_Debug(TAG, "URL for version file registered"); // Log a debug message.

    return DOWNLOADER_ERROR::OK; // Return success.
}

DOWNLOADER_ERROR Downloader::setTimeout(uint16_t timeOut)
{
    if (timeOut < 50 || timeOut > 60000)
    {
        Log_Error(TAG, "Timeout is out of range (50-60000)"); // Log an error if the timeout is out of the valid range.
        return DOWNLOADER_ERROR::INVALID_ARGUMENT;            // Return an error indicating an invalid argument.
    }
    _timeOut = timeOut; // Set the timeout value.

    Log_Debug(TAG, "Timeout registered"); // Log a debug message.

    return DOWNLOADER_ERROR::OK; // Return success.
}

Stream *Downloader::getStreamPtr()
{
    if (_streamSize == 0 || _wifiClient == nullptr || _httpClient == nullptr)
    {
        Log_Error(TAG, "Firmware is null"); // Log an error if the firmware or related resources are null.
        return nullptr;                     // Return a nullptr to indicate a problem.
    }
    return _wifiClient; // Return the WiFi client pointer.
}

int Downloader::getStreamSize()
{
    return _streamSize; // Return the size of the downloaded stream.
}

void Downloader::CreateWifiClient()
{
    if (strlen(_cACertificate) > 0)
    {
        _wifiClient = new WiFiClientSecure();                         // Create a secure WiFi client if a CA certificate is provided.
        ((WiFiClientSecure *)_wifiClient)->setCACert(_cACertificate); // Set the CA certificate.
    }
    else
        _wifiClient = new WiFiClient(); // Create a regular WiFi client if no CA certificate is provided.
}

DOWNLOADER_ERROR Downloader::CreateHttpClient(const char *url)
{
    _httpClient = new HTTPClient(); // Create an HTTP client instance.
    if (!_httpClient->begin(*_wifiClient, url))
    {
        Log_Error(TAG, "HTTPClient begin failed"); // Log an error if HTTP client creation fails.
        return DOWNLOADER_ERROR::UNKNOWN;          // Return an unknown error.
    }
    _httpClient->useHTTP10(true);                                    // Use HTTP/1.0 for compatibility.
    _httpClient->setTimeout(_timeOut);                               // Set the HTTP request timeout.
    _httpClient->setFollowRedirects(HTTPC_DISABLE_FOLLOW_REDIRECTS); // Disable HTTP redirects.
    _httpClient->setUserAgent("UpdateOTA-Downloader");               // Set the user agent header.
    _httpClient->addHeader("Cache-Control", "no-cache");             // Add a Cache-Control header.

    // TODO: Add additional features or configurations if needed.

    return DOWNLOADER_ERROR::OK; // Return success.
}
