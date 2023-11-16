#ifndef DOWNLOADER_HPP
#define DOWNLOADER_HPP

/**
 * @file    Downloader.hpp
 * @brief   Defines a class for downloading files from a URL using HTTP(S) protocol.
 *          The class implements the DownloaderInterface interface.
 *          The class uses the ESP32 WiFiClient and HTTPClient libraries.
 * @author Ronny Antoon
 */

#include <WiFiClient.h> // WiFiClient
#include <HTTPClient.h> // HTTPClient
#include <Stream.h>     // Stream
#include <MultiPrinterLoggerInterface.hpp>

#include "DownloaderInterface.hpp"

/**
 * @brief   The Downloader class implements the DownloaderInterface interface.
 */
class Downloader : public DownloaderInterface
{
public:
    /**
     * @brief Construct a new Downloader object
     */
    Downloader(MultiPrinterLoggerInterface *logger = nullptr);

    /**
     * @brief Destroy the Downloader object
     */
    ~Downloader() override;

    /**
     * @brief Downloads a file from the specified URL.
     *
     * @return DOWNLOADER_ERROR indicating the success or failure of the download.
     * Possible values:
     * - DOWNLOADER_ERROR::OK: Download succeeded.
     * - DOWNLOADER_ERROR::NO_URL_PROVIDED: URL for bin file is empty.
     * - DOWNLOADER_ERROR::DOWNLOAD_FAILED: Download failed.
     * - DOWNLOADER_ERROR::SERVER_ERROR: Server error.
     * - DOWNLOADER_ERROR::UNKNOWN: Unknown error.
     */
    DOWNLOADER_ERROR download() override;

    /**
     * @brief Retrieves the version information and stores it in the provided buffer.
     *
     * @param versionDis Pointer to the buffer where version information will be stored.
     * @param maxLen Maximum length of the version information buffer.
     * @return DOWNLOADER_ERROR indicating the success or failure of the operation.
     * Possible values:
     * - DOWNLOADER_ERROR::OK: Version information retrieved successfully.
     * - DOWNLOADER_ERROR::NO_URL_PROVIDED: URL for version file is empty.
     * - DOWNLOADER_ERROR::INVALID_ARGUMENT: Invalid argument provided.
     * - DOWNLOADER_ERROR::DOWNLOAD_FAILED: Download of version information failed.
     * - DOWNLOADER_ERROR::SERVER_ERROR: Server error.
     * - DOWNLOADER_ERROR::UNKNOWN: Unknown error.
     */
    DOWNLOADER_ERROR getVersion(char *versionDis, int maxLen) override;

    /**
     * @brief Sets the CA (Certificate Authority) certificate for secure connections.
     *
     * @param cACertificate The CA certificate to be set.
     * @return DOWNLOADER_ERROR indicating the success or failure of the operation.
     * Possible values:
     * - DOWNLOADER_ERROR::OK: CA certificate set successfully.
     * - DOWNLOADER_ERROR::INVALID_ARGUMENT: Invalid argument provided.
     */
    DOWNLOADER_ERROR setCA(const char *cACertificate) override;

    /**
     * @brief Sets the URL for the binary file to be downloaded.
     *
     * @param uRLForBinFile The URL of the binary file to be downloaded.
     * @return DOWNLOADER_ERROR indicating the success or failure of the operation.
     * Possible values:
     * - DOWNLOADER_ERROR::OK: URL for binary file set successfully.
     * - DOWNLOADER_ERROR::INVALID_ARGUMENT: Invalid argument provided.
     */
    DOWNLOADER_ERROR setURLForBin(const char *uRLForBinFile) override;

    /**
     * @brief Sets the URL for the version information file.
     *
     * @param uRLForVersionFile The URL of the version information file.
     * @return DOWNLOADER_ERROR indicating the success or failure of the operation.
     * Possible values:
     * - DOWNLOADER_ERROR::OK: URL for version file set successfully.
     * - DOWNLOADER_ERROR::INVALID_ARGUMENT: Invalid argument provided.
     */
    DOWNLOADER_ERROR setURLForVersion(const char *uRLForVersionFile) override;

    /**
     * @brief Sets the timeout for the download operation.
     *
     * @param timeOut The timeout value in milliseconds.
     * @return DOWNLOADER_ERROR indicating the success or failure of the operation.
     * Possible values:
     * - DOWNLOADER_ERROR::OK: Timeout registered successfully.
     * - DOWNLOADER_ERROR::INVALID_ARGUMENT: Timeout value is out of range.
     */
    DOWNLOADER_ERROR setTimeout(uint16_t timeOut) override;

    /**
     * @brief Returns a pointer to the stream for downloading data.
     *
     * @return A pointer to the stream for downloading data.
     */
    Stream *getStreamPtr() override;

    /**
     * @brief Gets the size of the downloaded stream.
     *
     * @return The size of the downloaded stream.
     */
    int getStreamSize() override;

private:
    char _cACertificate[CA_MAX_LENGTH + 1];      // CA certificate
    char _uRLForBinFile[URL_MAX_LENGTH + 1];     // URL for the binary file
    char _uRLForVersionFile[URL_MAX_LENGTH + 1]; // URL for the version information file
    uint16_t _timeOut;                           // Timeout for the download operation
    WiFiClient *_wifiClient;                     // WiFi client
    HTTPClient *_httpClient;                     // HTTP client
    int _streamSize;                             // Size of the downloaded stream

    /**
     * @brief Creates a WiFi client with or without SSL, depending on the presence of a CA certificate.
     */
    void CreateWifiClient();

    /**
     * @brief Creates an HTTP client and initializes it with the provided URL.
     *
     * @param url The URL to initialize the HTTP client with.
     * @return DOWNLOADER_ERROR indicating the success or failure of HTTP client creation.
     * Possible values:
     * - DOWNLOADER_ERROR::OK: HTTP client created successfully.
     * - DOWNLOADER_ERROR::UNKNOWN: HTTP client creation failed.
     */
    DOWNLOADER_ERROR CreateHttpClient(const char *url);

    MultiPrinterLoggerInterface *_logger; // Logger
};

#endif // DOWNLOADER_HPP
