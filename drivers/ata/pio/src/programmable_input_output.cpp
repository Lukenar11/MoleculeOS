/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file implements an ATA-PIO driver.

    This driver receives a relative LBA address, the number of sectors to be read, 
    and a pointer; it then reads these sectors from the 
    hard disk using programmable I/O and writes the data to RAM.

NOTES:
    Some methods are defined only in the header file so that the 
    compiler can inline them more easily.

    The 'delay()' method causes a delay of approximately 500 ns, 
    since the ATA standard requires a delay of 400 ns in some cases.
    The 'delay()' method works by performing a read operation over the ATA bus multiple times
    (each of which takes about 100 ns); 
    however, since the read operation does not take exactly 100 ns, 
    this method introduces a delay of 500 ns as a safety measure.
*/

#include "programmable_input_output.hpp"

namespace drivers::ata
{
    bool Programmable_Input_Output::poll_until_drq_or_error() noexcept {
        uint32_t timeout = 5'000'000;
        while (timeout--) {
            const uint8_t status = runtime::byte_input(status_port());

            if (status & ATA_ERR) [[unlikely]]
                return false;

            if (!(status & ATA_BSY) && (status & ATA_DRQ))
                return true;
        }

        return false;
    }

    bool Programmable_Input_Output::poll_disk_read(uint16_t*& dest_buffer,
                                                   uint32_t& command_count,
                                                   uint32_t& sectors_to_read,
                                                   uint32_t& relative_lba) 
                                                   noexcept {
        while (command_count > 0) {
            delay();

            if (!poll_until_drq_or_error()) [[unlikely]]
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

    bool Programmable_Input_Output::pio_disk_read(uint16_t*& dest_buffer, 
                                                  uint32_t& sectors_to_read,
                                                  uint32_t& relative_lba) 
                                                  noexcept {
        const uint32_t absolute_lba = relative_lba + lba_start_addr;
        uint32_t to_transfer = (sectors_to_read > CHUNK_SECTORS) 
                               ? CHUNK_SECTORS 
                               : sectors_to_read;

        if (!ata_pio_read_guard(partition_length, 
                                relative_lba, 
                                to_transfer)) [[unlikely]]
            return false;

        runtime::byte_output(io_port_base + 2, static_cast<uint8_t>(to_transfer));
        runtime::byte_output(io_port_base + 3, shift_and_mask(absolute_lba, 0, BYTE_MASK));
        runtime::byte_output(io_port_base + 4, shift_and_mask(absolute_lba, 8, BYTE_MASK));
        runtime::byte_output(io_port_base + 5, shift_and_mask(absolute_lba, 16, BYTE_MASK));
        runtime::byte_output(io_port_base + 6, shift_and_mask(absolute_lba, 
                                                              24, 
                                                              NIBBLE_MASK, 
                                                              MASTER));

        runtime::byte_output(status_port(), READ_SECTORS);

        if (!poll_disk_read(dest_buffer, 
                            to_transfer, 
                            sectors_to_read, 
                            relative_lba))
            return false;


        const uint8_t final_status = runtime::byte_input(status_port());
        return !(final_status & ATA_ERR || final_status & ATA_DF);
    }

    void Programmable_Input_Output::delay() noexcept {
        for (uint32_t i = 0; i < 5; ++i) 
            runtime::byte_input(status_port());
    }

    void Programmable_Input_Output::reset_driver(const uint16_t dcr_port) 
                                                 noexcept {
        runtime::byte_output(dcr_port, SRST);
        runtime::byte_output(dcr_port, DCR_DEFAULT);

        delay();

        uint32_t timeout = 5'000'000;
        while (timeout--) {
            const uint8_t status = runtime::byte_input(dcr_port);
            if ((status & BSY) == DRDY)
                break;
        }
    }

    void Programmable_Input_Output::init(const uint32_t partition_len,
                                         const uint32_t lba_start,
                                         const uint16_t io_base,
                                         const uint16_t device_ctrl,
                                         const uint8_t master_flags) noexcept {
        partition_length  = partition_len;
        lba_start_addr    = lba_start;
        io_port_base      = io_base;
        device_control    = device_ctrl;
        master_save_flags = master_flags;
    }

    bool Programmable_Input_Output::read(int32_t& sectors_to_read,
                                         uint16_t*& dest_buffer,
                                         uint32_t& relative_lba) noexcept {
        if (sectors_to_read < 0) [[unlikely]] {
            reset_driver(dcr_port());
            sectors_to_read = 0;

            return true;
        }

        if (static_cast<uint32_t>(sectors_to_read) > MAX_ALLOWED_SECTOR_COUNT) [[unlikely]]
            return false;

        const uint32_t max_sectors = partition_length - 1;
        if (relative_lba > max_sectors || 
            static_cast<uint32_t>(sectors_to_read) > max_sectors || 
            relative_lba > (max_sectors - static_cast<uint32_t>(sectors_to_read + 1))) {
            return false;
        }

        const uint8_t initial_status = runtime::byte_input(status_port());
        if (initial_status & ATA_BSY) {
            if (!poll_until_drq_or_error()) {
                reset_driver(dcr_port());
                return false;
            }
        }

        bool read_success = true;
        uint32_t sectors_count = static_cast<uint32_t>(sectors_to_read);
        while (sectors_count > 0) {
            read_success = pio_disk_read(dest_buffer, 
                                         sectors_count, 
                                         relative_lba);

            if (!read_success) [[unlikely]] {
                reset_driver(dcr_port());
                sectors_to_read = sectors_count;

                return false;
            }
        }
        
        sectors_to_read = sectors_count;
        return read_success;
    }
} // namespace drivers::ata
