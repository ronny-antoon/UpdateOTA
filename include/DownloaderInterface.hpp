#ifndef DOWNLOADER_INTERFACE_HPP
#define DOWNLOADER_INTERFACE_HPP

/**
 * @file    DownloaderInterface.hpp
 * @brief   Defines an interface for downloading files.
 * @author  Ronny Antoon
 */

#include <Stream.h> // Stream

#define URL_MAX_LENGTH 256 // Maximum length of the URL for the binary file. And the version file.
#define CA_MAX_LENGTH 4096 // Maximum length of the CA certificate.

/**
 * @brief Enum representing different error states for the download process.
 */
enum class DOWNLOADER_ERROR
{
    OK,               ///< Download completed successfully.
    NO_URL_PROVIDED,  ///< No URL provided.
    INVALID_ARGUMENT, ///< Invalid argument or configuration.
    DOWNLOAD_FAILED,  ///< Download failed.
    SERVER_ERROR,     ///< Server error.
    UNKNOWN,          ///< An unknown error occurred during the download.
};

/**
 * @brief Interface class for downloading files.
 *
 * This class defines an interface for downloading files.
 */
class DownloaderInterface
{
public:
    /**
     * @brief Virtual destructor for the DownloaderInterface class.
     */
    virtual ~DownloaderInterface() = default;

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
    virtual DOWNLOADER_ERROR download() = 0;

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
    virtual DOWNLOADER_ERROR getVersion(char *versionDis, int maxLen) = 0;

    /**
     * @brief Sets the CA (Certificate Authority) certificate for secure connections.
     *
     * @param cACertificate The CA certificate to be set.
     * @return DOWNLOADER_ERROR indicating the success or failure of the operation.
     * Possible values:
     * - DOWNLOADER_ERROR::OK: CA certificate set successfully.
     * - DOWNLOADER_ERROR::INVALID_ARGUMENT: Invalid argument provided.
     */
    virtual DOWNLOADER_ERROR setCA(const char *CACertificate) = 0;

    /**
     * @brief Sets the URL for the binary file to be downloaded.
     *
     * @param uRLForBinFile The URL of the binary file to be downloaded.
     * @return DOWNLOADER_ERROR indicating the success or failure of the operation.
     * Possible values:
     * - DOWNLOADER_ERROR::OK: URL for binary file set successfully.
     * - DOWNLOADER_ERROR::INVALID_ARGUMENT: Invalid argument provided.
     */
    virtual DOWNLOADER_ERROR setURLForBin(const char *URLForBinFile) = 0;

    /**
     * @brief Sets the URL for the version information file.
     *
     * @param uRLForVersionFile The URL of the version information file.
     * @return DOWNLOADER_ERROR indicating the success or failure of the operation.
     * Possible values:
     * - DOWNLOADER_ERROR::OK: URL for version file set successfully.
     * - DOWNLOADER_ERROR::INVALID_ARGUMENT: Invalid argument provided.
     */
    virtual DOWNLOADER_ERROR setURLForVersion(const char *URLForVersionFile) = 0;

    /**
     * @brief Sets the timeout for the download operation.
     *
     * @param timeOut The timeout value in milliseconds.
     * @return DOWNLOADER_ERROR indicating the success or failure of the operation.
     * Possible values:
     * - DOWNLOADER_ERROR::OK: Timeout registered successfully.
     * - DOWNLOADER_ERROR::INVALID_ARGUMENT: Timeout value is out of range.
     */
    virtual DOWNLOADER_ERROR setTimeout(uint16_t timeout = 8000) = 0;

    /**
     * @brief Gets the size of the downloaded stream.
     *
     * @return The size of the downloaded stream.
     */
    virtual Stream *getStreamPtr() = 0;

    /**
     * @brief   Returns the size of the downloaded content stream.
     * @return  The size of the downloaded content stream in bytes.
     */
    virtual int getStreamSize() = 0;
};

#endif // DOWNLOADER_INTERFACE_HPP