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

#include "../ata/pio.hpp"

namespace drivers::ata
{ 
    status_t 
    Programmable_Input_Output::validate_storage_access(_IN_ const uint32_t partition_length,
                                                       _IN_ const uint32_t relative_lba, 
                                                       _IN_ const uint32_t to_transfer) 
                                                       noexcept {
        status_t status;

        if (partition_length == 0) [[unlikely]] {
            status = status::ATA_NO_DEVICE;
            goto cleanup;
        }

        if (to_transfer == 0) [[unlikely]] {
            status = status::ATA_INVALID_SECTOR_COUNT;
            goto cleanup;
        }

        if (to_transfer > partition_length) [[unlikely]] {
            status = status::ATA_INVALID_SECTOR_COUNT;
            goto cleanup;
        }

        if (relative_lba > partition_length) [[unlikely]] {
            status = status::ATA_INVALID_LBA;
            goto cleanup;
        }

        if (relative_lba > partition_length - to_transfer) [[unlikely]] {
            status = status::ATA_INVALID_LBA;
            goto cleanup;
        }

        status = status::SUCCESS;

    cleanup:
        return status;
    }

    void 
    Programmable_Input_Output::delay() noexcept {
        for (uint32_t i = 0; i < 5; ++i) [[likely]] {
            stdlib::byte_input(status_port());
        }
    }

    void 
    Programmable_Input_Output::reset_driver(const uint16_t dcr_port) noexcept {
        uint32_t timeout = 5'000'000;
        uint8_t status;

        stdlib::byte_output(dcr_port, SRST);
        stdlib::byte_output(dcr_port, DCR_DEFAULT);

        delay();

        while (timeout--) [[likely]] {
            status = stdlib::byte_input(status_port());

            if (!(status & ATA_BSY) && (status & ATA_DRDY)) {
                break;
            }
        }
    }

    status_t
    Programmable_Input_Output::identify_drive(_OUT_ uint16_t identify_data[SECTOR_WORD_SIZE],
                                              _IN_  const uint16_t io_base,
                                              _IN_  const uint16_t control_reg,
                                              _IN_  const bool is_master)
                                              noexcept {
        status_t status;

        device_control_reg = control_reg;
        reset_driver(control_reg);

        io_port_base = io_base;

        if (is_master) {
            master_save_flags = MASTER_SELECT;
        }
        else {
            master_save_flags = SLAVE_SELECT;
        }

        stdlib::byte_output(status_port() - 1, master_save_flags);
        stdlib::byte_output(status_port(), ATA_IDENTIFY);

        delay();

        status = poll_until_drq_or_error();
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        stdlib::word_input_stream(io_base, SECTOR_WORD_SIZE, identify_data);

        status = status::SUCCESS;

    cleanup:
        return status;
    }

    status_t 
    Programmable_Input_Output::probe_and_configure_channel(_IN_ const uint16_t io_port,
                                                           _IN_ const uint16_t control_reg) 
                                                           noexcept {
        status_t status;
        stdlib::Array<uint16_t, SECTOR_WORD_SIZE> identify_data;
        uint32_t storage_size;
        bool is_master;

        for (uint32_t i = 0; i < 2; ++i) [[likely]] {
            is_master = (i == 0);

            status = identify_drive(identify_data.data(),
                                    io_port,
                                    control_reg,
                                    is_master);
            if (status != status::SUCCESS) [[unlikely]] {
                reset_driver(control_reg);
                continue;
            }

            storage_size = (static_cast<uint32_t>(identify_data[61]) << 16) |
                            static_cast<uint32_t>(identify_data[60]);

            io_port_base       = io_port;
            device_control_reg = control_reg;
            lba_start_address  = 0;

            if (is_master) {
                master_save_flags = MASTER;
            }
            else {
                master_save_flags = SLAVE;
            }

            if (storage_size == ATA_LBA28_OVERFLOW_MARKER) [[unlikely]] {
                partition_length = ATA_LBA28_MAX_ADDRESS;
            }
            else [[likely]] {
                partition_length = storage_size;
            }

            status = status::SUCCESS;
            goto cleanup;
        }

        status = status::ATA_NO_DEVICE;

    cleanup:
        return status;
    }

    status_t 
    Programmable_Input_Output::poll_until_drq_or_error() noexcept {
        status_t status;
        uint8_t input;
        uint32_t timeout = 5'000'000;

        while (timeout--) [[likely]] {
            input = stdlib::byte_input(status_port());

            if (input & ATA_ERR) [[unlikely]] {
                status = status::ATA_ERROR;
                goto cleanup;
            }

            if (input & ATA_DF) [[unlikely]] {
                status = status::ATA_DEVICE_FAULT;
                goto cleanup;
            }

            if (!(input & ATA_BSY) && (input & ATA_DRQ)) {
                status = status::SUCCESS;
                goto cleanup;
            }
        }

        status = status::ATA_TIMEOUT;

    cleanup:
        return status;
    }

    status_t 
    Programmable_Input_Output::poll_until_not_busy() noexcept {
        status_t status;
        uint8_t input;
        uint32_t timeout = 5'000'000;

        while (timeout--) [[likely]] {
            input = stdlib::byte_input(status_port());

            if (input & ATA_ERR) [[unlikely]] {
                status = status::ATA_ERROR;
                goto cleanup;
            }

            if (input & ATA_DF) [[unlikely]] {
                status = status::ATA_DEVICE_FAULT;
                goto cleanup;
            }

            if (!(input & ATA_BSY)) {
                status = status::SUCCESS;
                goto cleanup;
            }
        }

        status = status::ATA_TIMEOUT;

    cleanup:
        return status;
    }

    status_t 
    Programmable_Input_Output::poll_and_read_or_write_disk(_INOUT_ uint16_t* buffer,
                                                           _IN_    const Operations op,
                                                           _IN_    const uint32_t sector_count) 
                                                           noexcept {
        status_t status;

        for (uint32_t i = 0; i < sector_count; ++i) [[likely]] {
            delay();

            status = poll_until_drq_or_error();
            if (status != status::SUCCESS) [[unlikely]] {
                goto cleanup;
            }

            if (op == Operations::READ) {
                stdlib::word_input_stream(io_port_base,
                                          SECTOR_WORD_SIZE,
                                          buffer);

            } 
            else {
                stdlib::word_output_stream(io_port_base,
                                           SECTOR_WORD_SIZE,
                                           buffer);
            }

            buffer += SECTOR_WORD_SIZE;

            delay();
        }

        status = status::SUCCESS;

    cleanup:
        return status;
    }

    status_t 
    Programmable_Input_Output::start_pio_disk_read_or_write(_INOUT_ uint16_t* buffer,
                                                            _IN_    const Operations op,
                                                            _IN_    const uint32_t sector_count,
                                                            _IN_    const uint32_t relative_lba) 
                                                            noexcept {
        status_t status;
        uint32_t absolute_lba;
        uint8_t final_status;

        status = validate_storage_access(partition_length,
                                         relative_lba,
                                         sector_count);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        absolute_lba = relative_lba + lba_start_address;

        stdlib::byte_output(io_port_base + 2, 
                            static_cast<uint8_t>(sector_count));

        stdlib::byte_output(io_port_base + 3, 
                            (absolute_lba >> 0) & BYTE_MASK);

        stdlib::byte_output(io_port_base + 4, 
                            (absolute_lba >> 8) & BYTE_MASK);
                            
        stdlib::byte_output(io_port_base + 5, 
                            (absolute_lba >> 16) & BYTE_MASK);

        stdlib::byte_output(io_port_base + 6, 
                           (absolute_lba >> 24) & 
                           NIBBLE_MASK | master_save_flags);
                           
        if (op == Operations::READ) {
            stdlib::byte_output(status_port(), READ_SECTORS);
        }
        else {
            stdlib::byte_output(status_port(), WRITE_SECTORS);
        }

        status = poll_and_read_or_write_disk(buffer, op, sector_count);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        if (op == Operations::WRITE) {
            stdlib::byte_output(status_port(), FLUSH_CACHE);

            status = poll_until_not_busy();
            if (status != status::SUCCESS) [[unlikely]] {
                goto cleanup;
            }
        }

        final_status = stdlib::byte_input(status_port());

        if (final_status & ATA_ERR) [[unlikely]] {
            status = status::ATA_ERROR;
            goto cleanup;
        }

        if (final_status & ATA_DF) [[unlikely]] {
            status = status::ATA_DEVICE_FAULT;
            goto cleanup;
        }

        status = status::SUCCESS;

    cleanup:
        return status;
    }

    _API_ void 
    Programmable_Input_Output::init() noexcept {
        struct Channel final { 
            uint16_t io_port; 
            uint16_t control_reg; 
        };

        const Channel channels[] = { 
            {IDE_PRIMARY_IO_BASE,   IDE_PRIMARY_DCR_BASE}, 
            {IDE_SECONDARY_IO_BASE, IDE_SECONDARY_DCR_BASE} 
        };

        status_t status;
        for (const auto& channel : channels) [[likely]] {
            status = probe_and_configure_channel(channel.io_port, 
                                                 channel.control_reg);
            if (status == status::SUCCESS) {
                goto cleanup;
            }
        }

        kernel::sys::panic(
            "ATA init from 'IDENTIFY' failed, no device detected"
        );

    cleanup:
        return;
    }

    _API_ status_t 
    Programmable_Input_Output::run(_INOUT_ uint16_t* buffer,
                                   _IN_    uint32_t sector_count,
                                   _IN_    uint32_t relative_lba,
                                   _IN_    const Operations& operation) 
                                   noexcept {
        status_t status;
        uint32_t chunk;
        uint32_t max_sectors;

        if (sector_count == 0) [[unlikely]] {
            status = status::SUCCESS;
            goto cleanup;
        }

        if (sector_count > MAX_ALLOWED_SECTOR_COUNT) [[unlikely]] {
            status = status::ATA_INVALID_SECTOR_COUNT;
            goto cleanup;
        }

        max_sectors = partition_length - 1;

        if (relative_lba > max_sectors ||
            sector_count > max_sectors ||
            relative_lba > (max_sectors - sector_count + 1)) [[unlikely]] {
            status = status::ATA_INVALID_LBA;
            goto cleanup;
        }

        status = poll_until_not_busy();
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        while (sector_count > 0) [[likely]] {
            if (sector_count > CHUNK_SECTORS) {
                chunk = CHUNK_SECTORS;
            }
            else {
                chunk = sector_count;
            }

            status = start_pio_disk_read_or_write(buffer,
                                                  operation,
                                                  chunk,
                                                  relative_lba);
            if (status != status::SUCCESS) [[unlikely]] {
                goto cleanup;
            }

            buffer       += chunk * SECTOR_WORD_SIZE;
            relative_lba += chunk;
            sector_count -= chunk;
        }

        status = status::SUCCESS;

    cleanup:
        reset_driver(dcr_port());

        return status;
    }
} // namespace drivers::ata
