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

#include "programmable_io.hpp"

namespace drivers::ata
{
    bool Programmable_IO::poll_until_drq_or_error() noexcept {
        uint32_t timeout = 5'000'000;
        while (timeout--) {
            const uint8_t status = runtime::byte_input(status_port());
            if (status & ATA_ERR) 
                return false;

            if (!(status & ATA_DRDY) && (status & ATA_DRQ)) 
                return true;
        }

        return false;
    }

    bool Programmable_IO::read_and_poll_disk(uint16_t*& dest_buffer,
                                             uint32_t& command_count,
                                             uint32_t& sectors_to_read,
                                             uint32_t& relative_lba) noexcept {
        while (command_count > 0) {
            delay();

            if (!poll_until_drq_or_error())
                return false;

            runtime::word_input_stream(io_port_base,
                                       SECTOR_WORD_SIZE,
                                       dest_buffer);
            dest_buffer += SECTOR_WORD_SIZE;

            delay();

            relative_lba++;
            --command_count;
            --sectors_to_read;

            if (sectors_to_read == 0)
                break;
        }

        return true;
    }

    bool Programmable_IO::pio_28bit_read(uint16_t*& dest_buffer, 
                                         uint32_t& sectors_to_read,
                                         uint32_t& relative_lba) noexcept {
        const uint32_t absolute_lba = relative_lba + lba_start_addr;
        uint32_t to_transfer = (sectors_to_read > CHUNK_SECTORS) 
                               ? CHUNK_SECTORS 
                               : sectors_to_read;

        if (partition_length == 0) 
            return false;

        if (relative_lba > partition_length) 
            return false;

        if (to_transfer > partition_length) 
            return false;

        if (relative_lba > partition_length - to_transfer) 
            return false;

        runtime::byte_output(io_port_base + 2, static_cast<uint8_t>(to_transfer));
        runtime::byte_output(io_port_base + 3, shift_and_mask(absolute_lba, 0, BYTE_MASK));
        runtime::byte_output(io_port_base + 4, shift_and_mask(absolute_lba, 8, BYTE_MASK));
        runtime::byte_output(io_port_base + 5, shift_and_mask(absolute_lba, 16, BYTE_MASK));
        runtime::byte_output(io_port_base + 6, shift_and_mask(absolute_lba, 24, 
                                                                            (NIBBLE_MASK | 
                                                                             master_save_flags |
                                                                             0xE0)));

        runtime::byte_output(status_port(), READ_SECTORS);

        if (!read_and_poll_disk(dest_buffer, 
                                to_transfer, 
                                sectors_to_read, 
                                relative_lba))
            return false;


        const uint8_t final_status = runtime::byte_input(status_port());
        return !(final_status & ATA_ERR || final_status & ATA_DF);
    }

    bool Programmable_IO::pio_48bit_read(uint16_t*& dest_buffer, 
                                         uint32_t& sectors_to_read,
                                         uint32_t& relative_lba) noexcept {
        const uint32_t absolute_lba = relative_lba + lba_start_addr;
        uint32_t to_transfer = (sectors_to_read > CHUNK_SECTORS) 
                               ? CHUNK_SECTORS 
                               : sectors_to_read;

        if (partition_length == 0) 
            return false;

        if (relative_lba > partition_length) 
            return false;

        if (to_transfer > partition_length) 
            return false;

        if (relative_lba > partition_length - to_transfer) 
            return false;

        runtime::byte_output(io_port_base + 2, shift_and_mask(to_transfer, 8, BYTE_MASK));
        runtime::byte_output(io_port_base + 3, shift_and_mask(absolute_lba, 24, BYTE_MASK));
        runtime::byte_output(io_port_base + 4, shift_and_mask(absolute_lba, 32, BYTE_MASK));
        runtime::byte_output(io_port_base + 5, 0);

        runtime::byte_output(io_port_base + 2, shift_and_mask(to_transfer, 0, BYTE_MASK));
        runtime::byte_output(io_port_base + 3, shift_and_mask(absolute_lba, 0, BYTE_MASK));
        runtime::byte_output(io_port_base + 4, shift_and_mask(absolute_lba, 8, BYTE_MASK));
        runtime::byte_output(io_port_base + 5, shift_and_mask(absolute_lba, 16, BYTE_MASK));

        runtime::byte_output(io_port_base + 6, (master_save_flags & DRIVE_SELECT_BASE) | 
                                                DRDY);
        runtime::byte_output(status_port(), READ_SECTORS_EXT);

        if (!read_and_poll_disk(dest_buffer, 
                                to_transfer, 
                                sectors_to_read, 
                                relative_lba))
            return false;

        const uint8_t final_status = runtime::byte_input(status_port());
        return !(final_status & ATA_ERR || final_status & ATA_DF);
    }

    void Programmable_IO::delay() noexcept {
        for (uint32_t i = 0; i < 5; ++i) 
            runtime::byte_input(status_port());
    }

    void Programmable_IO::reset_driver(const uint16_t dcr_port) noexcept {
        runtime::byte_output(dcr_port, SRST);
        runtime::byte_output(dcr_port, DCR_DEFAULT);

        delay();

        while (true) {
            const uint8_t status = runtime::byte_input(dcr_port);
            if ((status & BSY) == DRDY)
                break;
        }
    }

    bool Programmable_IO::read(int32_t& sectors_to_read,
                               uint16_t*& dest_buffer,
                               uint32_t& relative_lba) noexcept {
        if (sectors_to_read < 0) {
            reset_driver(dcr_port());
            sectors_to_read = 0;

            return true;
        }

        if (static_cast<uint32_t>(sectors_to_read) > MAX_ALLOWED_SECTOR_COUNT)
            return false;

        const uint32_t max_sectors = partition_length - 1;
        if (relative_lba > max_sectors || 
            static_cast<uint32_t>(sectors_to_read) > max_sectors || 
            relative_lba > (max_sectors - static_cast<uint32_t>(sectors_to_read + 1))) {
            return false;
        }

        const uint8_t initial_status = runtime::byte_input(status_port());
        if (initial_status & ATA_DRDY) {
            if (!poll_until_drq_or_error()) {
                reset_driver(dcr_port());
                return false;
            }
        }

        bool requires_48bit_addressing = false;
        if (relative_lba > DOUBLE_WORD_MASK || 
            lba_start_addr > DOUBLE_WORD_MASK || 
            (lba_start_addr + relative_lba) > DOUBLE_WORD_MASK || 
            (lba_start_addr + relative_lba + sectors_to_read) > DOUBLE_WORD_MASK) {
            requires_48bit_addressing = true;
        }

        bool read_success = true;
        uint32_t sectors_count = static_cast<uint32_t>(sectors_to_read);
        while (sectors_count > 0) {
            if (requires_48bit_addressing)
                read_success = pio_48bit_read(dest_buffer, 
                                              sectors_count, 
                                              relative_lba);
            else
                read_success = pio_28bit_read(dest_buffer, 
                                              sectors_count, 
                                              relative_lba);

            if (!read_success) {
                reset_driver(dcr_port());
                sectors_to_read = sectors_count;
                return false;
            }
        }
        
        sectors_to_read = sectors_count;
        return read_success;
    }
} // namespace drivers::ata
