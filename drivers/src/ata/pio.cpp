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

    For the sake of simplicity, support for 48-bit LBA addresses has been omitted.
*/

#include "../ata/pio.hpp"

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
        const uint32_t absolute_lba = relative_lba + lba_start_address;
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

    bool Programmable_Input_Output::identify_drive(const uint16_t io_base,
                                                   const uint16_t control_reg,
                                                   const bool is_master,
                                                   uint16_t identify_data[SECTOR_WORD_SIZE])
                                                   noexcept {
        device_control_reg = control_reg;
        reset_driver(control_reg);

        io_port_base               = io_base;
        const uint8_t drive_select = (is_master) 
                                     ? MASTER_SELECT 
                                     : SLAVE_SELECT;

        runtime::byte_output(status_port() - 1, drive_select);
        delay();
        runtime::byte_output(status_port(), ATA_IDENTIFY);
        delay();
        
        if (!poll_until_drq_or_error()) [[unlikely]]
            return false;

        runtime::word_input_stream(io_base, SECTOR_WORD_SIZE, identify_data);
        return true;
    }

    bool Programmable_Input_Output::probe_and_configure_channel(const uint16_t io_port,
                                                                const uint16_t control_reg) 
                                                                noexcept {
        uint16_t identify_data[SECTOR_WORD_SIZE] = {0};
        for (int i = 0; i < 2; ++i) {
            const bool is_master = (i == 0);

            if (!identify_drive(io_port, 
                                control_reg, 
                                is_master, 
                                identify_data)) {
                reset_driver(control_reg);
                continue;
            }

            const uint32_t storage_size = (static_cast<uint32_t>(identify_data[61]) << 16) |
                                           static_cast<uint32_t>(identify_data[60]);

            io_port_base       = io_port;
            device_control_reg = control_reg;
            master_save_flags  = (is_master) ? MASTER : SLAVE;
            lba_start_address  = 0;

            if (storage_size == ATA_LBA28_OVERFLOW_MARKER)
                partition_length = ATA_LBA28_MAX_ADDRESS;
            else
                partition_length = storage_size;

            return true;
        }

        return false;
    }

    void Programmable_Input_Output::init() noexcept {
        struct Channel final { 
            uint16_t io_port; 
            uint16_t control_reg; 
        };

        const Channel channels[] = { 
            {IDE_PRIMARY_IO_BASE,   IDE_PRIMARY_DCR_BASE}, 
            {IDE_SECONDARY_IO_BASE, IDE_SECONDARY_DCR_BASE} 
        };

        for (const auto& channel : channels) {
            if (probe_and_configure_channel(channel.io_port, 
                                            channel.control_reg)) [[unlikely]]
                return;
        }

        kernel::system::panic("ATA init from IDENTIFY failed, no device detected");
    }

    bool Programmable_Input_Output::read(int32_t& sectors_to_read,
                                         uint16_t*& dest_buffer,
                                         uint32_t& relative_lba) noexcept {
        if (sectors_to_read < 0) [[unlikely]] {
            reset_driver(dcr_port());
            sectors_to_read = 0;

            return true;
        }

        if (static_cast<uint32_t>(sectors_to_read) >
            MAX_ALLOWED_SECTOR_COUNT) [[unlikely]]
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
