/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file implements an ATA PIO driver.

    This driver receives the relative LBA address, the number of sectors to be read, 
    and a pointer; it then reads these sectors from the hard drive 
    using programmable I/O and uses the pointer to write the read data to RAM.

NOTES:
    Some methods are defined only in the header so that the 
    compiler can inline them more easily.

    The 'delay()' method introduces a delay of approximately 500 ns, 
    since the ATA standard requires a 400ns delay in some cases.
    Because this method uses an ATA bus read operation (which takes approximately 100ns) 
    to generate the delay—rather than fixed timer logic—the additional approximately 100ns 
    is included as a safety margin.
*/

#pragma once

#include <stdint.h>
#include <port_io.hpp>

namespace drivers::ata
{
    class Programmable_IO final {
    private:
        static constexpr uint8_t ATA_BSY  = 0x80;
        static constexpr uint8_t ATA_DRQ  = 0x08;
        static constexpr uint8_t ATA_ERR  = 0x01;
        static constexpr uint8_t ATA_DF   = 0x20;
        static constexpr uint8_t ATA_DRDY = 0x40;

        static constexpr uint8_t SRST              = 0x04;
        static constexpr uint8_t DCR_DEFAULT       = 0x00;
        static constexpr uint8_t BSY               = 0xC0;
        static constexpr uint8_t DRDY              = 0x40;
        static constexpr uint8_t DRIVE_SELECT_BASE = 0x50;
        static constexpr uint8_t READ_SECTORS      = 0x20;
        static constexpr uint8_t READ_SECTORS_EXT  = 0x24;

        static constexpr uint8_t NIBBLE_MASK       = 0x0F;
        static constexpr uint8_t BYTE_MASK         = 0xFF;
        static constexpr uint32_t DOUBLE_WORD_MASK = 0x0FFFFFFF;

        static constexpr uint32_t SECTOR_WORD_SIZE = 0x0100;

        static constexpr uint32_t MAX_ALLOWED_SECTOR_COUNT = 128;
        static constexpr uint32_t CHUNK_SECTORS            = 8;
        static_assert(CHUNK_SECTORS <= MAX_ALLOWED_SECTOR_COUNT, 
                      "CHUNK_SECTORS <= MAX_ALLOWED_SECTOR_COUNT");

        static inline uint32_t partition_length = 0;
        static inline uint32_t lba_start_addr   = 0;
        static inline uint16_t device_control   = 0;
        static inline uint16_t io_port_base     = 0;
        static inline uint8_t master_save_flags = 0;

        static inline uint16_t status_port() noexcept { 
            return static_cast<uint16_t>(io_port_base + 7); 
        }

        static inline uint16_t dcr_port() noexcept { 
            return device_control; 
        }

        static bool poll_until_drq_or_error() noexcept;
        static bool read_and_poll_disk(uint16_t*& dest_buffer, 
                                       uint32_t& command_count, 
                                       uint32_t& sectors_to_read, 
                                       uint32_t& relative_lba) noexcept;

        static inline uint8_t shift_and_mask(const uint32_t val,
                                             const uint32_t shift,
                                             const uint32_t mask) noexcept {
            return static_cast<uint8_t>(((val >> shift) & mask));
        }


        static bool pio_28bit_read(uint16_t*& dest_buffer, 
                                   uint32_t& sectors_to_read, 
                                   uint32_t& relative_lba) noexcept;

        static bool pio_48bit_read(uint16_t*& dest_buffer, 
                                   uint32_t& sectors_to_read, 
                                   uint32_t& relative_lba) noexcept;

        static void delay() noexcept;
        static void reset_driver(const uint16_t dcr_port) noexcept;

    public:
        static bool read(int32_t& sectors_to_read, 
                         uint16_t*& dest_buffer, 
                         uint32_t& relative_lba) noexcept;
    };
} // namespace drivers::ata
