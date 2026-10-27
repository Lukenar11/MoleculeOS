/**
 * @brief Defines an driver for ATA-PIO storage access (PATA 40-Pin).
 *
 * @copyright Copyright (c) 2026 Lukenar11 (Luke Matthes)
 * @license:  https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
 * @author    Lukenar11 (Luke Matthes)
 *
 * @note The driver only supports LBA28, for the sake of simplicity.
 */


#pragma once

#include "interface.hpp"


namespace drivers::ata
{
    struct Programmable_IO_Operation final {
        uint16_t* buffer_ptr  = nullptr;
        uint32_t relative_lba = 0;
        uint32_t sector_count = 1;
    };


    class Programmable_IO : public Interface<Programmable_IO, 
                                             Programmable_IO_Operation> {
    private:
        static constexpr uint8_t ATA_BSY      = 0x80;
        static constexpr uint8_t ATA_DRQ      = 0x08;
        static constexpr uint8_t ATA_ERR      = 0x01;
        static constexpr uint8_t ATA_DF       = 0x20;
        static constexpr uint8_t ATA_DRDY     = 0x40;
        static constexpr uint8_t ATA_IDENTIFY = 0xEC;

        static constexpr uint8_t ATA_BBK   = 0x80;
        static constexpr uint8_t ATA_UNC   = 0x40;
        static constexpr uint8_t ATA_MC    = 0x20;
        static constexpr uint8_t ATA_IDNF  = 0x10;
        static constexpr uint8_t ATA_MCR   = 0x08;
        static constexpr uint8_t ATA_ABRT  = 0x04;
        static constexpr uint8_t ATA_TK0NF = 0x02;
        static constexpr uint8_t ATA_AMNF  = 0x01;

        static constexpr uint8_t SRST          = 0x04;
        static constexpr uint8_t DCR_DEFAULT   = 0x00;
        static constexpr uint8_t FLUSH_CACHE   = 0xE7;
        static constexpr uint8_t READ_SECTORS  = 0x20;
        static constexpr uint8_t WRITE_SECTORS = 0x30;

        static constexpr uint16_t IDE_PRIMARY_IO_BASE    = 0x1F0;
        static constexpr uint16_t IDE_PRIMARY_DCR_BASE   = 0x3F6;
        static constexpr uint16_t IDE_SECONDARY_IO_BASE  = 0x170;
        static constexpr uint16_t IDE_SECONDARY_DCR_BASE = 0x376;

        static constexpr uint8_t MASTER        = 0xE0;
        static constexpr uint8_t SLAVE         = 0xF0;
        static constexpr uint8_t MASTER_SELECT = 0xA0;
        static constexpr uint8_t SLAVE_SELECT  = 0xB0;

        static constexpr uint8_t NIBBLE_MASK = 0x0F;
        static constexpr uint8_t BYTE_MASK   = 0xFF;

        static constexpr uint32_t MAX_ALLOWED_SECTOR_COUNT = 256;
        static constexpr uint32_t CHUNK_SECTORS            = 8;

        static constexpr uint32_t MAX_RETRIES = 4;

        static constexpr uint32_t SECTOR_SIZE      = 512;                                
        static constexpr uint32_t SECTOR_WORD_SIZE = SECTOR_SIZE / 
                                                     sizeof(uint16_t);

        static constexpr uint32_t ATA_LBA28_OVERFLOW = 0xFFFFFFFF;
        static constexpr uint32_t ATA_LBA28_MAX      = 0x0FFFFFFF;

        enum class Driver_Action : uint8_t;

        uint32_t partition_length;
        uint32_t lba_start_address;
        uint16_t device_control_register;
        uint16_t io_port_base;
        uint8_t drive_select_flags;
        uint8_t lba_flags;
        uint16_t status_port;
        Driver_Action driver_action;


        static_assert(CHUNK_SECTORS <= MAX_ALLOWED_SECTOR_COUNT, 
                      "CHUNK_SECTORS <= MAX_ALLOWED_SECTOR_COUNT");


        /**
         * @brief Checks whether a memory access operation is within the
         *        allowed LBA range.
         *
         * @param partition_length Total number of sectors
         * @param relative_lba     Starting LBA address
         * @param to_transfer      Number of sectors to read or write.
         *
         * @retval `status::NULL_POINTER | status::flags::PARAM_A`
         *          If `buffer` is a `nullptr`.
         *
         * @retval `status::ATA_INVALID_LBA | status::flags::PARAM_B`
         *          If the LBA address exceeds the partition length or the
         *          LBA range (LBA + sectors) exceeds `partition_length`.
         *
         * @retval `status::ATA_INVALID_SECTOR_COUNT | status::flags::PARAM_C`
         *          If `to_transfer` is `0` or exceeds the `partition_length`.
         * 
         * @retval `status::ATA_NO_DEVICE`
         *          If the `partition_length` is `0`.
         *
         * @retval `status::SUCCESS`
         *          Default case.
         */
        [[nodiscard]] 
        status_t 
        validate_transfer_request(_IN_ const uint16_t* const buffer,
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
         * @param drive_select Drive/Head register base value that selects the
         *                     target device.
         */
        void
        issue_identify_command(_IN_ uint8_t drive_select) noexcept;


        /**
         * @brief Uses the ATA command `IDENTIFY` to retrieve the
         *        hard drive's identification data.
         *
         * @param identify_data    Output buffer for the `IDENTIFY` data
         * @param io_base          Base I/O port of the ATA channel
         * @param control_register Control register port of the ATA channel
         * @param drive_select     Drive/Head register base value that 
         *                         selects master or slave.
         * @param lba_mode         Drive/Head register base flags for 
         *                         LBA operation.
         *
         * @retval `status::INVALID_PARAMETER | status::flags::PARAM_A`
         *          If the pointer to `identify_data` is `nullptr`.
         *
         * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
         *          If `io_base` is not a valid ATA channel.
         *
         * @retval `status::INVALID_PARAMETER | status::flags::PARAM_C`
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
        identify_drive(_OUT_ uint16_t* const identify_data,
                       _IN_  const uint16_t io_base,
                       _IN_  const uint16_t control_register,
                       _IN_  const uint8_t drive_select,
                       _IN_  const uint8_t lba_mode) noexcept;


        /**
         * @brief Probes an ATA channel (master + slave) and configures the 
         *        driver if a valid hard drive is detected.
         *
         * @param io_port          Base I/O port of the ATA channel
         * @param control_register Control register port of the ATA channel
         *
         * @retval `status::INVALID_PARAMETER | status::flags::PARAM_A`
         *          If the I/O port is invalid.
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
         *          If `IDENTIFY` doesn't work.
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
         * @brief Polls the ATA status register until busy clears or 
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
         * @brief Performs a `PIO` read or write operation 
         *        for a given number of sectors.
         *
         * @param buffer       I/O buffer for sector data.
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
        transfer_sector_data(_INOUT_ uint16_t* buffer,
                             _IN_   const uint32_t sector_count) noexcept;


        /**
         * @brief Executes a complete ATA-PIO read or write operation starting 
         *        at a specific LBA address.
         *
         * @param buffer       I/O buffer for sector data.
         * @param relative_lba Starting LBA address.
         * @param sector_count Number of sectors to transfer.
         *
         * @retval `status::NULL_POINTER | status::flags::PARAM_A`
         *          If `buffer` is a `nullptr`.
         * 
         * @retval `status::ATA_INVALID_LBA | status::flags::PARAM_B`
         *          If the LBA range is invalid.
         *
         * @retval `status::ATA_INVALID_SECTOR_COUNT | status::flags::PARAM_C`
         *          If `sector_count` is invalid.
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
        start_pio_transfer(_INOUT_ uint16_t* const buffer,
                           _IN_    const uint32_t relative_lba,
                           _IN_    const uint32_t sector_count) noexcept;


        /**
         * @brief ATA-PIO main method performing chunked read/write
         *        operations over a specific sector range.
         *
         * @param buffer       I/O buffer for sector data
         * @param relative_lba Starting LBA address
         * @param sector_count Number of sectors to transfer
         * 
         * @note `sector_count`
         *        If `sector_count` is `0`, the `sector_count` is set to `256`.
         * 
         * @retval `status::NULL_POINTER | status::flags::PARAM_A`
         *          If `buffer` is a `nullptr`.
         *
         * @retval `status::ATA_INVALID_SECTOR_COUNT | status::flags::PARAM_B`
         *          If `sector_count` is `0` or exceeds the allowed maximum.
         *
         * @retval `status::ATA_INVALID_LBA | status::flags::PARAM_C`
         *          If the LBA range is invalid.
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
                                 _IN_    uint32_t relative_lba,
                                 _IN_    uint32_t sector_count) noexcept;


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
         */
        _API_
        status_t
        init() noexcept override;


        /**
         * @brief ATA-PIO main method performing chunked read
         *        operations over a specific sector range.
         *
         * @param operation Operation struct with buffer pointer,
         *                  start LBA and sector count.
         * 
         * @note `sector_count`
         *        If `sector_count` is `0`, the `sector_count` isset to 256.
         * 
         * @retval `status::NULL_POINTER | status::flags::PARAM_A`
         *          If `buffer` is a `nullptr`.
         *
         * @retval `status::ATA_INVALID_SECTOR_COUNT | status::flags::PARAM_B`
         *          If `sector_count` is `0` or exceeds the allowed maximum.
         *
         * @retval `status::ATA_INVALID_LBA | status::flags::PARAM_C`
         *          If the LBA range is invalid.
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
         * @note `sector_count`
         *        If `sector_count` is `0`, the `sector_count` isset to 256.
         * 
         * @retval `status::NULL_POINTER | status::flags::PARAM_A`
         *          If `buffer` is a `nullptr`.
         *
         * @retval `status::ATA_INVALID_SECTOR_COUNT | status::flags::PARAM_B`
         *          If `sector_count` is `0` or exceeds the allowed maximum.
         *
         * @retval `status::ATA_INVALID_LBA | status::flags::PARAM_C`
         *          If the LBA range is invalid.
         *
         * @retval `status::ATA_ERROR`
         *          If an error is reported during the write.
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
