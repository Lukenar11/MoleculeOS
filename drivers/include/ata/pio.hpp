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

    Some functions are defined only in the header file so that the 
    compiler can inline them more easily.

    The 'delay()' method causes a delay of approximately 500 ns, 
    since the ATA standard requires a delay of 400 ns in some cases.
    The 'delay()' method works by performing a read operation over the ATA bus multiple times
    (each of which takes about 100 ns); 
    however, since the read operation does not take exactly 100 ns, 
    this method introduces a delay of 500 ns as a safety measure.

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
    [[noreturn]]
    void panic(const char* message) noexcept;
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
        static inline uint8_t master_save_flags   = 0;

        [[nodiscard]] static
        uint8_t shift_and_mask(_IN_ const uint32_t val,
                               _IN_ const uint32_t shift,
                               _IN_ const uint8_t mask,
                               _IN_ const uint8_t add_mask=0x00) noexcept;

        [[nodiscard]] static
        status_t validate_storage_access(_IN_ const uint32_t partition_length, 
                                         _IN_ const uint32_t relative_lba, 
                                         _IN_ const uint32_t to_transfer) 
                                         noexcept;

        static 
        void delay() noexcept;

        static 
        void reset_driver(const uint16_t dcr_port) noexcept;

        static
        status_t identify_drive(_OUT_ uint16_t identify_data[SECTOR_WORD_SIZE],
                                _IN_  const uint16_t io_base,
                                _IN_  const uint16_t control_reg,
                                _IN_  const bool is_master) noexcept;

        static 
        status_t probe_and_configure_channel(_IN_ const uint16_t io_port,
                                             _IN_ const uint16_t control_reg) 
                                             noexcept;

        [[nodiscard]] static 
        status_t poll_until_drq_or_error() noexcept;

        [[nodiscard]] static 
        status_t poll_until_not_busy() noexcept;

        [[nodiscard]] static 
        status_t poll_and_read_or_write_disk(_INOUT_ uint16_t* buffer,
                                             _IN_    const Operations op,
                                             _IN_    const uint32_t sector_count) 
                                             noexcept;

        [[nodiscard]] static
        status_t start_pio_disk_read_or_write(_INOUT_ uint16_t* buffer,
                                              _IN_    const Operations op, 
                                              _IN_    const uint32_t sector_count,
                                              _IN_    const uint32_t relative_lba) 
                                              noexcept;

    public:
        [[nodiscard]] static inline 
        uint16_t status_port() noexcept { 
            return static_cast<uint16_t>(io_port_base + 7); 
        }

        [[nodiscard]] static inline 
        uint16_t dcr_port() noexcept { 
            return device_control_reg; 
        }

        static 
        void init() noexcept;

        static 
        status_t run(_INOUT_ uint16_t* buffer,
                     _IN_    uint32_t sector_count,
                     _IN_    uint32_t relative_lba,
                     _IN_    const Operations op) noexcept;

        Programmable_Input_Output() noexcept  = default;
        ~Programmable_Input_Output() noexcept = default;
    };
} // namespace drivers::ata
