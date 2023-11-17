#ifndef UPDATE_OTA_INTERFACE_HPP
#define UPDATE_OTA_INTERFACE_HPP

#include <stdint.h> // uint8_t

#define BLOCK_SIZE_P (4096) // Size of the block to write to the partition.

/**
 * @brief Enum representing different update OTA errors
 */
enum UpdateOTAError
{
    SUCCESS,                ///< Update process completed successfully
    NO_INTERNET,            ///< No internet connection available
    BAD_REQUEST,            ///< Bad request error during update
    UNAUTHORIZED,           ///< Unauthorized access during update
    PAGE_NOT_FOUND,         ///< Page not found error during update
    NO_PARTITION_AVAILABLE, ///< No available partition for update
    PARTITION_NOT_BOOTABLE, ///< Selected partition is not bootable
    UPDATE_PROGRESS_ERROR,  ///< Error during the update progress
    NO_ENOUGH_SPACE,        ///< Insufficient space for the update
    UNKNOWN,                ///< Unknown error during update
};

/**
 * @brief Abstract class defining the interface for handling Over-The-Air (OTA) updates
 */
class UpdateOTAInterface
{
public:
    /**
     * @brief Start the OTA update process with the specified URL, update type, and pin status
     * @param uRL The URL of the update
     * @param isFirmware Flag indicating whether the update is for firmware
     * @param pinStatus The pin status for controlling an LED during the update
     * @return UpdateOTAError indicating the success or failure of the OTA update process, Options:-
     *      UpdateOTAError::SUCCESS                 - If the OTA update process completed successfully
     *      UpdateOTAError::NO_INTERNET             - If there is no internet connection
     *      UpdateOTAError::BAD_REQUEST             - If there is a bad request error during update
     *      UpdateOTAError::UNAUTHORIZED            - If there is unauthorized access during update
     *      UpdateOTAError::PAGE_NOT_FOUND          - If the page is not found during update
     *      UpdateOTAError::NO_PARTITION_AVAILABLE  - If there is no available partition for update
     *      UpdateOTAError::PARTITION_NOT_BOOTABLE  - If the selected partition is not bootable
     *      UpdateOTAError::UPDATE_PROGRESS_ERROR   - If there is an error in the update progress
     *      UpdateOTAError::NO_ENOUGH_SPACE         - If there is insufficient space for the update
     *      UpdateOTAError::UNKNOWN                 - If there is an unknown error during update
     */
    virtual UpdateOTAError startUpdate(const char *uRL, bool isFirmware, uint8_t pinStatus) = 0;

    /**
     * @brief Get the version number from the specified URL and store it in the provided buffer
     * @param uRL The URL to get the version number from
     * @param buffer Buffer to store the version number
     * @param bufferSize Size of the buffer
     * @return UpdateOTAError indicating the success or failure of the operation, Options:-
     *      UpdateOTAError::SUCCESS         - If the version number was retrieved successfully
     *      UpdateOTAError::NO_INTERNET     - If there is no internet connection
     *      UpdateOTAError::BAD_REQUEST     - If there is a bad request error during update
     *      UpdateOTAError::UNAUTHORIZED    - If there is unauthorized access during update
     *      UpdateOTAError::PAGE_NOT_FOUND  - If the page is not found during update
     *      UpdateOTAError::NO_ENOUGH_SPACE - If there is insufficient space for the update
     *      UpdateOTAError::UNKNOWN         - If there is an unknown error during update
     */
    virtual UpdateOTAError getVersionNumber(const char *uRL, char *buffer, uint8_t bufferSize) = 0;

    /**
     * @brief Convert an update OTA error to a human-readable string and store it in the provided buffer
     * @param error The update OTA error
     * @param buffer Buffer to store the error message
     * @param bufferSize Size of the buffer
     */
    virtual void errorToString(UpdateOTAError error, char *buffer, uint8_t bufferSize) = 0;
};

#endif // UPDATE_OTA_INTERFACE_HPP
