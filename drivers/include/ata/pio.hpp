/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file implements an ATA-PIO driver.

    This driver contains the operation to be performed (READ/WRITE), 
    a pointer, the number of sectors, the relative 'LBA-address', 
    and will either perform a hard drive read operation or a hard drive write operation.

NOTES:
    Some methods are defined only in the header file so that the 
    compiler can inline them more easily.

    For the sake of simplicity, support for 48-bit 'LBA-addresses' has been omitted.

    The forward-declaration for the 'kernel::sys::panic();' function is intends, 
    because the inclusion of the '<kernel.hpp>' regarding conflicts with the 
    '<drivers.hpp>'.
*/


#pragma once

#include "../utils/ata_pio_helpers.hpp"
#include <types.hpp>
#include <sal.hpp>
#include <status.hpp>
#include <port_io.hpp>
#include <array.hpp>


namespace kernel::sys
{
    [[noreturn]] void
    panic(const char* message) noexcept;

    extern "C" {
        void disable_interrupts();
        void enable_interrupts();
    }
} // namespace kernel::sys


namespace drivers::ata
{
    class Programmable_Input_Output final {
    private:
        static constexpr uint8_t ATA_BSY         = 0x80;
        static constexpr uint8_t ATA_DRQ         = 0x08;
        static constexpr uint8_t ATA_ERR         = 0x01;
        static constexpr uint8_t ATA_DF          = 0x20;
        static constexpr uint8_t ATA_DRDY        = 0x40;
        static constexpr uint8_t ATA_IDENTIFY    = 0xEC;

        static constexpr uint32_t ATA_LBA28_OVERFLOW_MARKER = 0xFFFFFFFF;
        static constexpr uint32_t ATA_LBA28_MAX_ADDRESS     = 0x0FFFFFFF;

        static constexpr uint8_t SRST              = 0x04;
        static constexpr uint8_t DCR_DEFAULT       = 0x00;
        static constexpr uint8_t BSY               = 0xC0;
        static constexpr uint8_t DRDY              = 0x40;
        static constexpr uint8_t DRIVE_SELECT_BASE = 0x50;
        static constexpr uint8_t FLUSH_CACHE       = 0xE7;
        static constexpr uint8_t FLUSH_CACHE_EXT   = 0xEA;
        static constexpr uint8_t READ_SECTORS      = 0x20;
        static constexpr uint8_t READ_SECTORS_EXT  = 0x24;
        static constexpr uint8_t WRITE_SECTORS     = 0x30;
        static constexpr uint8_t WRITE_SECTORS_EXT = 0x34;

        static constexpr uint16_t IDE_PRIMARY_IO_BASE    = 0x1F0;
        static constexpr uint16_t IDE_PRIMARY_DCR_BASE   = 0x3F6;
        static constexpr uint16_t IDE_SECONDARY_IO_BASE  = 0x170;
        static constexpr uint16_t IDE_SECONDARY_DCR_BASE = 0x376;

        static constexpr uint8_t MASTER        = 0xE0;
        static constexpr uint8_t SLAVE         = 0xF0;
        static constexpr uint8_t MASTER_SELECT = 0xA0;
        static constexpr uint8_t SLAVE_SELECT  = 0xB0;

        static constexpr uint8_t NIBBLE_MASK       = 0x0F;
        static constexpr uint8_t BYTE_MASK         = 0xFF;
        static constexpr uint32_t DOUBLE_WORD_MASK = 0x0FFFFFFF;

        static constexpr uint32_t MAX_ALLOWED_SECTOR_COUNT = 128;
        static constexpr uint32_t CHUNK_SECTORS            = 8;
        static_assert(CHUNK_SECTORS <= MAX_ALLOWED_SECTOR_COUNT, 
                      "CHUNK_SECTORS <= MAX_ALLOWED_SECTOR_COUNT");

        static inline uint32_t partition_length   = 0;
        static inline uint32_t lba_start_address  = 0;
        static inline uint16_t device_control_reg = 0;
        static inline uint16_t io_port_base       = 0;
        static inline uint8_t drive_select_flags  = 0;
        static inline uint8_t lba_flags           = 0;


        /**
         * @brief Checks whether a memory access operation is within the
         *        allowed LBA range.
         *
         * @param partition_length Total number of sectors
         * @param relative_lba     Starting LBA address
         * @param to_transfer      Number of sectors to read or write.
         *
         * @retval `status::ATA_NO_DEVICE`
         *         If the `partition_length` is `0`.
         *
         * @retval `status::ATA_INVALID_LBA | status::flags::PARAM_B`
         *         If the LBA address exceeds the partition length.
         *
         * @retval `status::ATA_INVALID_LBA | status::flags::PARAM_B`
         *         If the LBA range (LBA + sectors) exceeds `partition_length`.
         *
         * @retval `status::ATA_INVALID_SECTOR_COUNT | status::flags::PARAM_C`
         *         If `to_transfer` is `0` or exceeds the `partition_length`.
         *
         * @retval `status::SUCCESS`
         *         Default case.
         */
        [[nodiscard]] static status_t 
        validate_storage_access(_IN_ const uint32_t relative_lba, 
                                _IN_ const uint32_t to_transfer) noexcept;


        /**
         * @brief Introduces a delay of ~500 ns.
         *
         * @note The ATA specification requires a minimum delay of 400 ns 
         *       after certain operations.
         * 
         * @note This implementation performs five consecutive read operations from the
         *       status port, each lasting ~100 ns, to ensure a safe delay is achieved.
         * 
         * @note Since the operation being performed does not 
         *       generate a delay of exactly 100 ns, 
         *       a 500 ns delay is executed as a precaution to 
         *       ensure a minimum delay of around 400 ns.
         */
        static void 
        delay() noexcept;


        /**
         * @brief Performs a software reset of the hard drive and waits
         *        until the controller is operational again.
         *
         * @param dcr_port Port of the ATA channel's hard drive status register.
         */
        static void 
        reset_driver(const uint16_t dcr_port) noexcept;


        /**
         * @brief Uses the ATA command `IDENTIFY` to retrieve the
         *        hard drive's identification data.
         *
         * @param identify_data Output buffer for the `IDENTIFY` data
         * @param io_base       Base I/O port of the ATA channel
         * @param control_reg   Control register port of the ATA channel
         *
         * @retval `status::INVALID_PARAMETER | status::flags::PARAM_A`
         *         If the pointer to `identify_data` is `nullptr`.
         *
         * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
         *         If `io_base` is not a valid ATA channel.
         *
         * @retval `status::INVALID_PARAMETER | status::flags::PARAM_C`
         *         If the `control_reg` port is invalid.
         *
         * @retval `status::ATA_ERROR`
         *         If an error is reported during the `IDENTIFY` operation.
         *
         * @retval `status::ATA_DEVICE_FAULT`
         *         If the device enters an error state.
         *
         * @retval `status::ATA_TIMEOUT`
         *         If no response is received in the timeout period.
         *
         * @retval `status::SUCCESS`
         *         Default case.
         */
        static status_t 
        identify_drive(_OUT_ uint16_t identify_data[SECTOR_WORD_SIZE],
                       _IN_  const uint16_t io_base,
                       _IN_  const uint16_t control_reg) noexcept;


        /**
         * @brief Probes an ATA channel (master + slave) and configures the driver
         *        if a valid hard drive is detected.
         *
         * @param io_port     Base I/O port of the ATA channel
         * @param control_reg Control register port of the ATA channel
         *
         * @retval `status::INVALID_PARAMETER | status::flags::PARAM_A`
         *         If the I/O port is invalid.
         *
         * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
         *         If the control register port is invalid.
         * 
         * @retval `status::ATA_ERROR`
         *         If an error is reported during the IDENTIFY operation.
         *
         * @retval `status::ATA_DEVICE_FAULT`
         *         If the device enters an error state.
         *
         * @retval `status::ATA_TIMEOUT`
         *         If no response is received in the timeout period.
         *
         * @retval `status::ATA_NO_DEVICE`
         *         If neither master nor slave responds to IDENTIFY.
         *
         * @retval `status::SUCCESS`
         *         If a device was successfully detected and configured.
         */
        static status_t 
        probe_and_configure_channel(_IN_ const uint16_t io_port,
                                    _IN_ const uint16_t control_reg) noexcept;


        /**
         * @brief Polls the ATA status register until `DRQ` is set or 
         *        an error appeared.
         *
         * @retval `status::ATA_ERROR`
         *         If the hard drive sets the `ERR` bit.
         *
         * @retval `status::ATA_DEVICE_FAULT`
         *         If the hard drive sets the `DF` bit.
         *
         * @retval `status::ATA_TIMEOUT`
         *         If no response is received in the timeout period.
         *
         * @retval `status::SUCCESS`
         *         Default case.
         */
        [[nodiscard]] static status_t 
        poll_until_drq_or_error() noexcept;


        /**
         * @brief Polls the ATA status register until `BSY` clears or 
         *        an error appeared.
         *
         * @retval `status::ATA_ERROR`
         *         If the hard drive sets the `ERR` bit.
         *
         * @retval `status::ATA_DEVICE_FAULT`
         *         If the hard drive sets the `DF` bit.
         *
         * @retval `status::ATA_TIMEOUT`
         *         If no response is received in the timeout period.
         *
         * @retval `status::SUCCESS`
         *         Default case.
         */
        [[nodiscard]] static status_t 
        poll_until_not_bsy_or_error() noexcept;


        /**
         * @brief Performs a `PIO` read or write operation 
         *        for a given number of sectors.
         *
         * @param buffer       I/O buffer for sector data.
         * @param op           Operation (`READ` or `WRITE`).
         * @param sector_count Number of sectors to transfer.
         *
         * @retval `status::ATA_INVALID_SECTOR_COUNT | status::flags::PARAM_C`
         *         If `sector_count` is `0`.
         *
         * @retval `status::ATA_ERROR`
         *         If the hard drive reports an error during transfer.
         *
         * @retval `status::ATA_DEVICE_FAULT`
         *         If the hard drive sets the `DF`-bit.
         *
         * @retval `status::ATA_TIMEOUT`
         *         If no response is received in the timeout period.
         *
         * @retval `status::SUCCESS`
         *         Default case.
         */
        [[nodiscard]] static status_t
        poll_and_read_or_write_disk(_INOUT_ uint16_t* buffer,
                                     _IN_    const Operations op,
                                     _IN_    const uint32_t sector_count) 
                                     noexcept;


        /**
         * @brief Executes a complete ATA-PIO read or write operation starting at
         *        a specific LBA address.
         *
         * @param buffer       I/O buffer for sector data.
         * @param op           Operation type (READ or WRITE).
         * @param sector_count Number of sectors to transfer.
         * @param relative_lba Starting LBA address.
         *
         * @retval `status::ATA_INVALID_LBA | status::flags::PARAM_B`
         *         If the LBA range is invalid.
         *
         * @retval `status::ATA_INVALID_SECTOR_COUNT | status::flags::PARAM_C`
         *         If `sector_count` is invalid.
         *
         * @retval `status::ATA_ERROR`
         *         If the hard drive reports an error.
         *
         * @retval `status::ATA_DEVICE_FAULT`
         *         If the hard drive sets the `DF` bit.
         *
         * @retval `status::ATA_TIMEOUT`
         *         If no response is received in the timeout period.
         *
         * @retval `status::SUCCESS`
         *         Default case.
         */
        [[nodiscard]] static status_t 
        start_pio_disk_read_or_write(_INOUT_ uint16_t* buffer,
                                     _IN_    const uint32_t relative_lba,
                                     _IN_    const uint32_t sector_count,
                                     _IN_    const Operations op) 
                                     noexcept;


    public:
        /**
         * @brief Returns the I/O port address of the ATA status register.
         *
         * @retval The computed status port address.
         */
        _API_ [[nodiscard]] static inline uint16_t 
        status_port() noexcept { 
            return static_cast<uint16_t>(io_port_base + 7); 
        }


        /**
         * @brief Returns the I/O port address of the ATA device control register.
         *
         * @retval The computed status port address.
         */
        _API_ [[nodiscard]] static inline uint16_t 
        dcr_port() noexcept { 
            return device_control_reg; 
        }


        /**
         * @brief Initializes the ATA-PIO driver.
         */
        _API_ static void 
        init() noexcept;

        
        /**
         * @brief ATA-PIO interface performing chunked read/write
         *        operations over a specific sector range.
         *
         * @param buffer       I/O buffer for sector data
         * @param sector_count Number of sectors to transfer
         * @param relative_lba Starting LBA address
         * @param operation    Operation (`READ` or `WRITE`)
         *
         * @retval `status::ATA_INVALID_SECTOR_COUNT | status::flags::PARAM_B`
         *         If the sector count is zero or exceeds the allowed maximum.
         *
         * @retval `status::ATA_INVALID_LBA | status::flags::PARAM_C`
         *         If the LBA range is invalid.
         *
         * @retval `status::ATA_ERROR`
         *         If the hard drive reports an error.
         *
         * @retval `status::ATA_DEVICE_FAULT`
         *         If the hard drive sets the `DF` bit.
         *
         * @retval `status::ATA_TIMEOUT`
         *         If no response is received in the timeout period.
         *
         * @retval `status::SUCCESS`
         *         Default case.
         */
        _API_ static status_t 
        run(_INOUT_ uint16_t* buffer,
            _IN_    uint32_t sector_count,
            _IN_    uint32_t relative_lba,
            _IN_    const Operations& operation) noexcept;


        Programmable_Input_Output() noexcept  = default;
        ~Programmable_Input_Output() noexcept = default;
    };
} // namespace drivers::ata
