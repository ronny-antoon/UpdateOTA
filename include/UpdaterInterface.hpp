#ifndef UPDATER_INTERFACE_HPP
#define UPDATER_INTERFACE_HPP

/**
 * @file    UpdaterInterface.hpp
 * @brief   Interface class for performing updates.
 * @author  Ronny Antoon
 */

#include <Stream.h> // Stream

#define BLOCK_SIZE_P (4096) // Partition block size

/**
 * @brief Enum representing different error states for the update process.
 */
enum class UPDATER_ERROR
{
    OK,                           ///< Update completed successfully.
    NO_PARTITION_AVAILABLE,       ///< No partition available for update.
    CHANGE_BOOT_PARTITION_FAILED, ///< Failed to change the boot partition.
    PARTITION_WRITE_FAILED,       ///< Failed to write to the partition.
    READ_FAILED,                  ///< Failed to read from the update data.
    NO_ENOUGH_SPACE,              ///< Not enough space for the update.
    UNKNOWN                       ///< An unknown error occurred during the update.
};

/**
 * @brief Enum representing the type of update (firmware or SPIFFS).
 */
enum class UPDATER_TYPE
{
    FIRMWARE, ///< Firmware update.
    SPIFFS    ///< SPIFFS update.
};

/**
 * @brief Interface class for performing updates.
 *
 * This class defines an interface for performing firmware and SPIFFS updates on ESP32.
 */
class UpdaterInterface
{
public:
    /**
     * @brief Virtual destructor for the UpdaterInterface class.
     */
    virtual ~UpdaterInterface() = default;

    /**
     * @brief Set a callback function to be invoked at the start of an update.
     *
     * @param onStart_cb A function pointer to the callback function.
     */
    virtual void setOnStartFunc(void (*onStart_cb)()) = 0;

    /**
     * @brief Set a callback function to be invoked during the update progress.
     *
     * @param onProgress_cb A function pointer to the callback function.
     */
    virtual void setOnProgressFunc(void (*onProgress_cb)(int, int)) = 0;

    /**
     * @brief Set a callback function to be invoked at the end of an update.
     *
     * @param onEnd_cb A function pointer to the callback function.
     */
    virtual void setOnEndFunc(void (*onEnd_cb)()) = 0;

    /**
     * @brief Set a callback function to be invoked in case of an error during the update.
     *
     * @param onError_cb A function pointer to the callback function.
     */
    virtual void setOnErrorFunc(void (*onError_cb)(int)) = 0;

    /**
     * @brief Start the update process.
     *
     * @param streamData A pointer to a Stream object containing update data.
     * @param streamLength The length of the update data stream.
     * @param type The type of update (FIRMWARE or SPIFFS).
     * @return UPDATER_ERROR An error code indicating the result of the update process:
     *          - UPDATER_ERROR::OK: Update completed successfully.
     *          - UPDATER_ERROR::NO_PARTITION_AVAILABLE: No partition available for update.
     *          - UPDATER_ERROR::CHANGE_BOOT_PARTITION_FAILED: Failed to change the boot partition.
     *          - UPDATER_ERROR::PARTITION_WRITE_FAILED: Failed to write to the partition.
     *          - UPDATER_ERROR::READ_FAILED: Failed to read from the update data.
     *          - UPDATER_ERROR::NO_ENOUGH_SPACE: Not enough space for the update.
     *          - UPDATER_ERROR::UNKNOWN: An unknown error occurred during the update.
     */
    virtual UPDATER_ERROR startUpdate(Stream *streamData, int streamLength, UPDATER_TYPE type) = 0;
};

#endif // UPDATER_INTERFACE_HPP
