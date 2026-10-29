/**
 * @brief Defines an driver for ATA-PIO storage access (PATA 40-Pin).
 *
 * @copyright Copyright (c) 2026 Lukenar11 (Luke Matthes)
 * @license:  https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
 * @author    Lukenar11 (Luke Matthes)
 *
 * @note The driver only supports 28-Bit LBA's, for the sake of simplicity.
 */


#pragma once

#include "interface.hpp"


namespace drivers::ata
{
    /**
     * @brief Represents an ATA-PIO driver operation.
     * 
     * @note `buffer`
     *        is set to `nullptr` by default.
     * 
     * @note `buffer_size` 
     *        is set to `0` by default.
     * 
     * @note `relative_lba`
     *        is set to `0` by default.
     * 
     * @note `sector_count`
     *        is set to `0` by default.
     */
    struct Programmable_IO_Operation final {
        uint16_t* buffer      = nullptr;
        uint32_t buffer_size  = 0;
        uint32_t relative_lba = 0;
        uint32_t sector_count = 0;
    };

    /**
     * @brief Provides an ATA-PIO driver for storage access.
     * 
     * @details Implements storage access to the 
     *          classic PATA 40-Pin hard drives.
     * 
     * @details Supports data transfer from RAM to the hard drive and
     *          data transfer from the hard drive to the RAM,
     *          based on a starting LBA28 address and 
     *          a number of sectors to be transferred.
     */
    class Programmable_IO : public Interface<Programmable_IO, 
                                             Programmable_IO_Operation> {
    private:
        static constexpr uint32_t MAX_ALLOWED_SECTOR_COUNT = 256;
        static constexpr uint32_t CHUNK_SECTORS            = 8;

        static constexpr uint32_t MAX_RETRIES = 4;

        static constexpr uint32_t SECTOR_SIZE      = 512;                                
        static constexpr uint32_t SECTOR_WORD_SIZE = SECTOR_SIZE / 
                                                     sizeof(uint16_t);

        static constexpr uint32_t ATA_LBA28_OVERFLOW  = 0xFFFFFFFF;
        static constexpr uint32_t ATA_LBA28_MAX       = 0x0FFFFFFF;
        static constexpr uint16_t ATA_LBA_SUPPORT_BIT = 0x0200;

        static constexpr uint8_t NIBBLE_MASK = 0x0F;
        static constexpr uint8_t BYTE_MASK   = 0xFF;

        enum class ATA_Status : uint8_t;
        enum class ATA_Error : uint8_t;
        enum class ATA_Command : uint8_t;
        enum class ATA_Control : uint8_t;
        enum class ATA_Port : uint16_t;
        enum class ATA_Device : uint8_t;

        enum class Driver_Action : uint8_t;

        Driver_Action driver_action;

        struct Identify_Data final {
            uint32_t partition_length        = 0;
            uint32_t lba_start_address       = 0;
            uint16_t device_control_register = 0;
            uint16_t io_port_base            = 0;
            uint16_t error_port              = 0;
            uint16_t status_port             = 0;
            uint8_t drive_select_flags       = 0;
            uint8_t lba_flags                = 0;
            bool lba_supported               = false;
        }
        identify_data;


        static_assert(CHUNK_SECTORS <= MAX_ALLOWED_SECTOR_COUNT, 
                      "CHUNK_SECTORS <= MAX_ALLOWED_SECTOR_COUNT");


        /**
         * @brief Checks whether a memory access operation is within the
         *        allowed LBA range.
         *
         * @param buffer       Pointer to a buffer for sector data.
         * @param buffer_size  Buffer byte size.
         * @param relative_lba LBA start address.
         * @param to_transfer  Number of sectors to read or write.
         *
         * @retval `status::NULL_POINTER | status::flags::PARAM_A`
         *          If `buffer` is a `nullptr`.
         * 
         * @retval `status::BUFFER_TOO_SMALL | status::flags::PARAM_B`
         *          If `buffer_size` is too small for the requested transfer.
         *
         * @retval `status::ATA_INVALID_LBA | status::flags::PARAM_C`
         *          If `relative_lba` is outside the valid partition range or 
         *          the requested transfer range exceeds the partition length.
         *
         * @retval `status::ATA_INVALID_SECTOR_COUNT | status::flags::PARAM_D`
         *          If `to_transfer` is `0` or exceeds the 
         *          allowed maximum sectorcount.
         * 
         * @retval `status::ATA_NO_DEVICE`
         *          If no device is configured.
         *
         * @retval `status::SUCCESS`
         *          Default case.
         */
        [[nodiscard]] 
        status_t 
        validate_transfer_request(_IN_ const uint16_t* const buffer,
                                  _IN_ const uint32_t buffer_size,
                                  _IN_ const uint32_t relative_lba, 
                                  _IN_ const uint32_t to_transfer) noexcept;


        /**
         * @brief Introduces a delay of ~400 ns.
         *
         * @note The ATA specification requires a minimum delay of 400 ns 
         *       after certain operations.
         * 
         * @details The delay is implemented by reading the 
         *          ATA bus multiple times (each read operation taking ~100ns).
         * 
         * @details Since a read operation does not take exactly 100ns,
         *          a delay of 500 ns is introduced as a safety measure.
         */
        void 
        delay() noexcept;


        /**
         * @brief Performs a software reset of the hard drive and waits
         *        until the controller is operational again.
         */
        void 
        reset_driver() noexcept;


        /**
         * @brief Issues the ATA command `IDENTIFY` for the 
         *        currently selected drive.
         *
         * @param drive_select Drive/Head register value, 
         *                     selecting master or slave.
         */
        void
        issue_identify_command(_IN_ uint8_t drive_select) noexcept;


        /**
         * @brief Handles retryable ATA errors and updates the retry counter.
         * 
         * @param retries 
         *        Retry counter that may be adjusted for certain errors.
         * 
         * @return `true/false` 
         *          If the ATA error is not retryable.
         */
        [[nodiscard]]
        bool
        handle_retryable_ata_error(_INOUT_ int32_t& retries) noexcept;


        /**
         * @brief Tries to identify a single device on the 
         *        already configured ATA channel, 
         *        with retries and error handling.
         *
         * @param id_command_buffer Pointer to a buffer for the 
         *                          `IDENTIFY` data.
         * @param drive_select      Drive/Head register value,
         *                          selecting master or slave.
         *
         * @retval `status::INVALID_PARAMETER | status::flags::PARAM_A`
         *          If `id_command_buffer` is a `nullptr`.
         *
         * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
         *          If `drive_select` is invalid.
         *
         * @retval `status::ATA_ERROR`
         *          If an unrecoverable ATA error is reported.
         *
         * @retval `status::ATA_DEVICE_FAULT`
         *          If the hard drive enters an error state.
         *
         * @retval `status::ATA_TIMEOUT`
         *          If no response is received in the timeout period.
         *
         * @retval `status::SUCCESS`
         *          Default case.
         */
        [[nodiscard]]
        status_t
        try_drive_identify(_OUT_ uint16_t* const id_command_buffer,
                           _IN_  const uint8_t drive_select) noexcept;


        /**
         * @brief Uses the ATA command `IDENTIFY` to retrieve the
         *        hard drive's identification data.
         *
         * @param id_command_buffer Pointer to a buffer for the 
         *                          `IDENTIFY` data.
         * @param drive_select      Drive/Head register value, 
         *                          selecting master or slave.
         * @param io_base           Base I/O port of the ATA channel.
         * @param control_register  Control register port of the ATA channel.
         * @param lba_mode          Drive/Head register base flags for the
         *                          LBA operation.
         *
         * @retval `status::INVALID_PARAMETER | status::flags::PARAM_A`
         *          If the pointer to `id_command_buffer` is `nullptr`.
         * 
         * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B` 
         *          If `drive_select` is invalid.
         *
         * @retval `status::INVALID_PARAMETER | status::flags::PARAM_C`
         *          If `io_base` is not a valid ATA channel.
         *
         * @retval `status::INVALID_PARAMETER | status::flags::PARAM_D`
         *          If the `control_register` port is invalid.
         *
         * @retval `status::ATA_ERROR`
         *          If an error is reported during the `IDENTIFY` operation.
         *
         * @retval `status::ATA_DEVICE_FAULT`
         *          If the hard drive enters an error state.
         *
         * @retval `status::ATA_TIMEOUT`
         *          If no response is received in the timeout period.
         *
         * @retval `status::SUCCESS`
         *          Default case.
         */
        [[nodiscard]] 
        status_t 
        identify_drive(_OUT_ uint16_t* const id_command_buffer,
                       _IN_  const uint8_t drive_select,
                       _IN_  const uint16_t io_base,
                       _IN_  const uint16_t control_register,
                       _IN_  const uint8_t lba_mode) noexcept;


        /**
         * @brief Probes an ATA channel (master + slave) and configures the 
         *        driver if a valid hard drive is detected.
         *
         * @param io_port          Base I/O port of the ATA channel.
         * @param control_register Control register port of the ATA channel.
         *
         * @retval `status::INVALID_PARAMETER | status::flags::PARAM_A`
         *          If `io_port` is not a valid ATA channel.
         *
         * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
         *          If the control register port is invalid.
         * 
         * @retval `status::ATA_ERROR`
         *          If an error is reported during the `IDENTIFY` operation.
         *
         * @retval `status::ATA_DEVICE_FAULT`
         *          If the device enters an error state.
         *
         * @retval `status::ATA_TIMEOUT`
         *          If no response is received in the timeout period.
         *
         * @retval `status::ATA_NO_DEVICE`
         *          If no channel is configured.
         *
         * @retval `status::SUCCESS`
         *          Default case.
         */
        [[nodiscard]]
        status_t 
        probe_and_configure_channel(_IN_ const uint16_t io_port,
                                    _IN_ const uint16_t control_register) 
                                    noexcept;


        /**
         * @brief Polls the ATA status register until device ready is set or 
         *        an error appeared.
         *
         * @retval `status::ATA_ERROR`
         *          If an error is reported during the polls.
         *
         * @retval `status::ATA_DEVICE_FAULT`
         *          If the device enters an error state.
         *
         * @retval `status::ATA_TIMEOUT`
         *          If no response is received in the timeout period.
         *
         * @retval `status::SUCCESS`
         *          Default case.
         */
        [[nodiscard]] 
        status_t 
        wait_until_ready_or_error() noexcept;


        /**
         * @brief Polls the ATA status register until device ready is set or 
         *        an error appeared.
         *
         * @retval `status::ATA_ERROR`
         *          If an error is reported during the polls.
         *
         * @retval `status::ATA_DEVICE_FAULT`
         *          If the device enters an error state.
         *
         * @retval `status::ATA_TIMEOUT`
         *          If no response is received in the timeout period.
         *
         * @retval `status::SUCCESS`
         *          Default case.
         */
        [[nodiscard]] 
        status_t 
        wait_until_unbusy_or_error() noexcept;


        /**
         * @brief Waits until the ATA status register indicates that a sector
         *        transfer can proceed.
         * 
         * @param buffer       Pointer to a buffer for the sector data.
         * @param sector_count Number of sectors to transfer.
         * 
         * @retval `status::NULL_POINTER | status::flags::PARAM_A`
         *          If `buffer` is a `nullptr`.
         * 
         * @retval `status::ATA_INVALID_SECTOR_COUNT | status::flags::PARAM_B`
         *          If `sector_count` is `0`.
         *
         * @retval `status::ATA_ERROR`
         *          If an error is reported during the polls.
         *
         * @retval `status::ATA_DEVICE_FAULT`
         *          If the device enters an error state.
         *
         * @retval `status::ATA_TIMEOUT`
         *          If no response is received in the timeout period.
         *
         * @retval `status::SUCCESS`
         *          Default case.
         */
        [[nodiscard]]
        status_t
        wait_until_sector_transfer_ready(_INOUT_ uint16_t* buffer, 
                                         _IN_    const uint32_t sector_count)
                                         noexcept;


        /**
         * @brief Performs a `PIO` read or write operation 
         *        for a given number of sectors.
         *
         * @param buffer       Pointer to a buffer for the sector data.
         * @param sector_count Number of sectors to transfer.
         *
         * @retval `status::NULL_POINTER | status::flags::PARAM_A`
         *          If `buffer` is a `nullptr`.
         * 
         * @retval `status::ATA_INVALID_SECTOR_COUNT | status::flags::PARAM_B`
         *          If `sector_count` is `0`.
         * 
         * @retval `status::ATA_ERROR`
         *          If an error is reported during the transfer or polls.
         *
         * @retval `status::ATA_DEVICE_FAULT`
         *          If the device enters an error state.
         *
         * @retval `status::ATA_TIMEOUT`
         *          If no response is received in the timeout period.
         *
         * @retval `status::SUCCESS`
         *          Default case.
         */
        [[nodiscard]] 
        status_t
        transfer_sector_data(_INOUT_ uint16_t* buffer,
                             _IN_   const uint32_t sector_count) noexcept;


        /**
         * @brief Executes a complete ATA-PIO read or write operation starting 
         *        at a specific LBA address.
         *
         * @param buffer       Pointer to a buffer for sector data.
         * @param buffer_size  Buffer byte size.
         * @param relative_lba LBA start address.
         * @param sector_count Number of sectors to transfer.
         *
         * @retval `status::NULL_POINTER | status::flags::PARAM_A`
         *          If `buffer` is a `nullptr`.
         * 
         * @retval `status::BUFFER_TOO_SMALL | status::flags::PARAM_B` 
         *          If `buffer_size` is too small for the requested transfer.
         *
         * @retval `status::ATA_INVALID_LBA | status::flags::PARAM_C`
         *          If `relative_lba` is outside the valid partition range or 
         *          the requested transfer range exceeds the partition length.
         *
         * @retval `status::ATA_INVALID_SECTOR_COUNT | status::flags::PARAM_D`
         *          If `to_transfer` is `0` or exceeds the 
         *          allowed maximum sectorcount.
         * 
         * @retval `status::ATA_NO_DEVICE`
         *          If no device is configured.
         * 
         * @retval `status::ATA_ERROR` 
         *          If an error is reported during the polls.
         *
         * @retval `status::ATA_DEVICE_FAULT`
         *          If the device enters an error state.
         *
         * @retval `status::ATA_TIMEOUT`
         *          If no response is received in the timeout period.
         *
         * @retval `status::SUCCESS`
         *          Default case.
         */
        [[nodiscard]] 
        status_t 
        start_pio_transfer(_INOUT_ uint16_t* const buffer,
                           _IN_    const uint32_t buffer_size,
                           _IN_    const uint32_t relative_lba,
                           _IN_    const uint32_t sector_count) noexcept;



        /**
         * @brief Tries to transfer one sector chunk, 
         *        with error and retries handling.
         *
         * @param buffer       Pointer to a buffer for sector data.
         * @param buffer_size  Buffer byte size.
         * @param relative_lba LBA start address.
         * @param chunk        Number of sectors in this transfer chunk.
         *
         * @retval `status::NULL_POINTER | status::flags::PARAM_A`
         *          If `buffer` is a `nullptr`.
         * 
         * @retval `status::BUFFER_TOO_SMALL | status::flags::PARAM_B` 
         *          If buffer_size is too small for the requested transfer.
         *
         * @retval `status::ATA_INVALID_LBA | status::flags::PARAM_C`
         *          If `relative_lba` is outside the valid partition range or 
         *          the requested transfer range exceeds the partition length.
         *
         * @retval `status::ATA_INVALID_SECTOR_COUNT | status::flags::PARAM_D`
         *          If `to_transfer` is `0` or exceeds the 
         *          allowed maximum sectorcount.
         * 
         * @retval `status::ATA_NO_DEVICE`
         *          If no device is configured.
         * 
         * @retval `status::ATA_ERROR` 
         *          If an error is reported during the polls.
         *
         * @retval `status::ATA_DEVICE_FAULT`
         *          If the device enters an error state.
         *
         * @retval `status::ATA_TIMEOUT`
         *          If no response is received in the timeout period.
         *
         * @retval `status::SUCCESS`
         *          Default case.
         */
        [[nodiscard]]
        status_t
        try_chunk_transfer(_INOUT_ uint16_t* buffer,
                           _IN_    uint32_t buffer_size,
                           _IN_    uint32_t relative_lba,
                           _IN_    uint32_t chunk) noexcept;


        /**
         * @brief ATA-PIO main method performing chunked read/write
         *        operations over a specific sector range.
         *
         * @param buffer       Pointer to a buffer for sector data.
         * @param relative_lba LBA start address.
         * @param sector_count Number of sectors to transfer.
         * 
         * @note If `sector_count` is `0`, the `sector_count` is set to `256`.
         * 
         * @retval `status::NULL_POINTER | status::flags::PARAM_A`
         *          If `buffer` is a `nullptr`.
         *
         * @retval `status::ATA_INVALID_LBA | status::flags::PARAM_B`
         *          If `relative_lba` is outside the valid partition range or 
         *          the requested transfer range exceeds the partition length.
         *
         * @retval `status::ATA_INVALID_SECTOR_COUNT | status::flags::PARAM_C`
         *          If `sector_count` exceeds the allowed maximum sectorcount.
         * 
         * @retval `status::ATA_NO_DEVICE`
         *          If no device is configured.
         *
         * @retval `status::ATA_ERROR`
         *          If an error is reported during the transfer.
         *
         * @retval `status::ATA_DEVICE_FAULT`
         *          If the device enters an error state.
         *
         * @retval `status::ATA_TIMEOUT`
         *          If no response is received in the timeout period.
         *
         * @retval `status::SUCCESS`
         *          Default case.
         */ 
        [[nodiscard]]
        status_t 
        execute_transfer_request(_INOUT_ uint16_t* buffer,
                                 _IN_    uint32_t buffer_size,
                                 _IN_    uint32_t relative_lba,
                                 _IN_    uint32_t sector_count) noexcept;


        /**
         * @brief Initializes one ATA channel and 
         *        retries probing on transient errors.
         *
         * @param io_port          Base I/O port of the ATA channel.
         * @param control_register Control register port of the ATA channel.
         *
         * @retval `status::INVALID_PARAMETER | status::flags::PARAM_A`
         *          If `io_port` is not a valid ATA channel.
         *
         * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
         *          If the `control_register` port is invalid.
         *
         * @retval `status::ATA_NO_DEVICE`
         *          If no device is configured.
         *
         * @retval `status::ATA_ERROR`
         *          If probing fails with an unrecoverable ATA error.
         *
         * @retval `status::ATA_DEVICE_FAULT`
         *          If the device enters an error state.
         *
         * @retval `status::ATA_TIMEOUT`
         *          If no response is received in the timeout period.
         *
         * @retval `status::SUCCESS`
         *          Default case.
         */
        [[nodiscard]]
        status_t
        init_channel(_IN_ const uint16_t io_port, 
                     _IN_ const uint16_t control_register) noexcept;


    public:
        /**
         * @brief Nulls the ATA-PIO driver.
         */
        _API_
        Programmable_IO() noexcept;


        /**
         * @brief Uninitializes the ATA-PIO driver.
         * 
         * @note Is declared as default.
         */
        _API_
        ~Programmable_IO() noexcept = default;


        /**
         * @brief Initializes the ATA-PIO driver.
         * 
         * @retval `status::ATA_NO_DEVICE`
         *          If no device is found.
         *
         * @retval `status::ATA_ERROR`
         *          If initialisation fails with an unrecoverable ATA error.
         *
         * @retval `status::ATA_DEVICE_FAULT`
         *          If the device enters an error state.
         *
         * @retval `status::ATA_TIMEOUT`
         *          If no response is received in the timeout period.
         * 
         * @retval `status::FAIL`
         *          Internal driver error.
         *
         * @retval `status::SUCCESS`
         *          Default case.
         */
        _API_
        [[nodiscard]]
        status_t
        init() noexcept override;


        /**
         * @brief ATA-PIO main method performing chunked read
         *        operations over a specific sector range.
         *
         * @param operation Operation struct with buffer pointer,
         *                  start LBA and sector count.
         * 
         * @note If `operation.sector_count` is `0`, 
         *       the `operation.sector_count` isset to `256`.
         * 
         * @retval `status::NULL_POINTER | status::flags::PARAM_A`
         *          If `operation.buffer` is a `nullptr`.
         *
         * @retval `status::ATA_INVALID_SECTOR_COUNT | status::flags::PARAM_A`
         *          If `operation.sector_count` exceeds the 
         *          allowed maximum sectorcount.
         *
         * @retval `status::ATA_INVALID_LBA | status::flags::PARAM_A`
         *          If `operation.relative_lba` is outside the 
         *          valid partition range or the 
         *          requested transfer range exceeds the partition length.
         *
         * @retval `status::ATA_NO_DEVICE` 
         *          If no device is configured.
         * 
         * @retval `status::ATA_ERROR`
         *          If an error is reported during the read.
         *
         * @retval `status::ATA_DEVICE_FAULT`
         *          If the device enters an error state.
         *
         * @retval `status::ATA_TIMEOUT`
         *          If no response is received in the timeout period.
         *
         * @retval `status::SUCCESS`
         *          Default case.
         */
        _API_
        [[nodiscard]]
        status_t
        load_sectors(_INOUT_ Programmable_IO_Operation& operation) 
                    noexcept override;


        /**
         * @brief ATA-PIO main method performing chunked write
         *        operations over a specific sector range.
         *
         * @param operation Operation struct with buffer pointer,
         *                  start LBA and sector count.
         * 
         * @note If `operation.sector_count` is `0`, 
         *       the `operation.sector_count` isset to `256`.
         * 
         * @retval `status::NULL_POINTER | status::flags::PARAM_A`
         *          If `operation.buffer` is a `nullptr`.
         *
         * @retval `status::ATA_INVALID_SECTOR_COUNT | status::flags::PARAM_A`
         *          If `operation.sector_count` exceeds the 
         *          allowed maximum sectorcount.
         *
         * @retval `status::ATA_INVALID_LBA | status::flags::PARAM_A`
         *          If `operation.relative_lba` is outside the 
         *          valid partition range or the 
         *          requested transfer range exceeds the partition length.
         *
         * @retval `status::ATA_NO_DEVICE` 
         *          If no device is configured.
         * 
         * @retval `status::ATA_ERROR`
         *          If an error is reported during the read.
         *
         * @retval `status::ATA_DEVICE_FAULT`
         *          If the device enters an error state.
         *
         * @retval `status::ATA_TIMEOUT`
         *          If no response is received in the timeout period.
         *
         * @retval `status::SUCCESS`
         *          Default case.
         */
        _API_
        [[nodiscard]]
        status_t
        store_sectors(_INOUT_ Programmable_IO_Operation& operation) 
                      noexcept override;


        /**
         * @brief Gets the size of one sector.
         * 
         * @note Can be called without any driver initialisation.
         * 
         * @retval sector byte size
         */
        _API_
        [[nodiscard]]
        static
        inline
        uint32_t
        sector_size() noexcept {
            return SECTOR_SIZE;
        }
    };
} // namespace drivers::ata
