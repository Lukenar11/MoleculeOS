/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file implements an ATA-PIO driver.

    This driver contains the operation to be performed (READ/WRITE), 
    a pointer, the number of sectors, the relative 'LBA-address', 
    and will either perform a hard drive read operation or 
    a hard drive write operation.

NOTES:
    Some methods are defined only in the header file so that the 
    compiler can inline them more easily.

    For the sake of simplicity, support for 48-bit 'LBA-addresses' 
    has been omitted.

    The forward-declaration for the 
    'kernel::sys::panic();' function is intends, 
    because the inclusion of the '<kernel.hpp>' regarding conflicts with the 
    '<drivers.hpp>'.
*/


#include "pio.hpp"
#include <kernel.hpp>


namespace drivers::ata
{
    enum class Programmable_IO::Driver_Operation : uint8_t {
        NONE  = 0x00,
        READ  = 0x01,
        WRITE = 0x02
    };


    /**
     * @brief Checks whether a memory access operation is within the
     *        allowed LBA range.
     *
     * @param buffer           Pointer to the I/O buffer.
     * @param partition_length Total number of sectors.
     * @param relative_lba     Starting LBA address.
     * @param to_transfer      Number of sectors to read or write.
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *         If `buffer` is a `nullptr`.
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
    [[nodiscard]] 
    status_t
    Programmable_IO::validate_storage_access(_IN_ const uint16_t* const buffer,
                                             _IN_ const uint32_t relative_lba,
                                             _IN_ const uint32_t to_transfer)
                                             noexcept {
        status_t status;

        if (!buffer) [[unlikely]] {
            status = status::NULL_POINTER | status::flags::PARAM_A;
            goto cleanup;
        }

        if (partition_length == 0) [[unlikely]] {
            status = status::ATA_NO_DEVICE;
            goto cleanup;
        }

        if (to_transfer == 0 ||
            to_transfer > partition_length ||
            to_transfer > MAX_ALLOWED_SECTOR_COUNT) [[unlikely]] {
            status = status::ATA_INVALID_SECTOR_COUNT | status::flags::PARAM_C;
            goto cleanup;
        }

        if (relative_lba >= partition_length ||
            relative_lba > (partition_length - to_transfer)) [[unlikely]] {
            status = status::ATA_INVALID_LBA | status::flags::PARAM_B;
            goto cleanup;
        }

        status = status::SUCCESS;

    cleanup:
        return status;
    }


    /**
     * @brief Introduces a delay of ~500 ns.
     *
     * @note The ATA specification requires a minimum delay of 400 ns 
     *       after certain operations.
     * 
     * @note This implementation performs five consecutive read 
     *       operations from the status port, 
     *       each lasting ~100 ns, to ensure a safe delay is achieved.
     * 
     * @note Since the operation being performed does not 
     *       generate a delay of exactly 100 ns, 
     *       a 500 ns delay is executed as a precaution to 
     *       ensure a minimum delay of around 400 ns.
     */
    void 
    Programmable_IO::delay() noexcept {
        for (uint32_t ns_delay = 0; ns_delay < 5; ns_delay++) [[likely]] {
            stdlib::byte_input(status_port);
        }
    }


    /**
     * @brief Performs a software reset of the hard drive and waits
     *        until the controller is operational again.
     */
    void 
    Programmable_IO::reset_driver() noexcept {
        uint32_t timeout = 5'000'000;
        uint8_t status;

        stdlib::byte_output(device_control_register, SRST);
        stdlib::byte_output(device_control_register, DCR_DEFAULT);

        delay();

        while (timeout--) [[likely]] {
            status = stdlib::byte_input(status_port);

            if (!(status & ATA_BSY) && (status & ATA_DRDY)) {
                break;
            }
        }
    }


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
     *         If the pointer to `identify_data` is `nullptr`.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
     *         If `io_base` is not a valid ATA channel.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_C`
     *         If the `control_register` port is invalid.
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
    [[nodiscard]]
    status_t
    Programmable_IO::identify_drive(_OUT_ uint16_t* const identify_data,
                                    _IN_  const uint16_t io_base,
                                    _IN_  const uint16_t control_register,
                                    _IN_  const uint8_t drive_select,
                                    _IN_  const uint8_t lba_mode)
                                    noexcept {
        status_t status;

        if (!identify_data) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_A;
            goto cleanup;
        }

        if (io_base != IDE_PRIMARY_IO_BASE &&
            io_base != IDE_SECONDARY_IO_BASE) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_B;
            goto cleanup;
        }

        if (control_register != IDE_PRIMARY_DCR_BASE &&
            control_register != IDE_SECONDARY_DCR_BASE) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_C;
            goto cleanup;
        }

        io_port_base            = io_base;
        status_port             = io_port_base + 7;
        device_control_register = control_register;
        drive_select_flags      = drive_select;
        lba_flags               = lba_mode;

        reset_driver();

        stdlib::byte_output(io_port_base + 6, drive_select_flags);
        delay();

        stdlib::byte_output(status_port, ATA_IDENTIFY);
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


    /**
     * @brief Probes an ATA channel (master + slave) and configures the driver
     *        if a valid hard drive is detected.
     *
     * @param io_port          Base I/O port of the ATA channel
     * @param control_register Control register port of the ATA channel
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
    [[nodiscard]]
    status_t
    Programmable_IO::probe_and_configure_channel(_IN_ const uint16_t io_port,
                                                 _IN_ const uint16_t control_register)
                                                 noexcept {
        status_t status;
        stdlib::Array<uint16_t, SECTOR_WORD_SIZE> identify_data;
        uint32_t storage_size;

        const struct Device_Config final {
            uint8_t drive_select = 0;
            uint8_t lba_mode     = 0;
        }
        devices[] = {
            {
                MASTER_SELECT, 
                MASTER
            },
            {
                SLAVE_SELECT,  
                SLAVE
            }
        };

        if (io_port != IDE_PRIMARY_IO_BASE &&
            io_port != IDE_SECONDARY_IO_BASE) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_A;
            goto cleanup;
        }

        if (control_register != IDE_PRIMARY_DCR_BASE &&
            control_register != IDE_SECONDARY_DCR_BASE) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_B;
            goto cleanup;
        }

        for (const auto& device : devices) [[likely]] {
            status = identify_drive(identify_data.data(),
                                    io_port,
                                    control_register,
                                    device.drive_select,
                                    device.lba_mode);

            if (status != status::SUCCESS) [[unlikely]] {
                reset_driver();
                continue;
            }

            storage_size = (static_cast<uint32_t>(identify_data[61]) << 16) |
                            static_cast<uint32_t>(identify_data[60]);

            io_port_base            = io_port;
            device_control_register = control_register;
            lba_start_address       = 0;

            if (storage_size != ATA_LBA28_OVERFLOW_MARKER) [[likely]] {
                partition_length = storage_size;
            }
            else [[unlikely]] {
                partition_length = ATA_LBA28_MAX_ADDRESS;
            }

            status = status::SUCCESS;
            goto cleanup;
        }

        status = status::ATA_NO_DEVICE;

    cleanup:
        return status;
    }


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
    [[nodiscard]] 
    status_t 
    Programmable_IO::poll_until_drq_or_error() noexcept {
        status_t status;
        uint8_t input;
        uint32_t timeout = 5'000'000;

        while (timeout--) [[likely]] {
            input = stdlib::byte_input(status_port);

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
    [[nodiscard]]
    status_t 
    Programmable_IO::poll_until_not_bsy_or_error() noexcept {
        status_t status;
        uint8_t input;
        uint32_t timeout = 5'000'000;

        while (timeout--) [[likely]] {
            input = stdlib::byte_input(status_port);

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


    /**
     * @brief Performs a `PIO` read or write operation 
     *        for a given number of sectors.
     *
     * @param buffer       I/O buffer for sector data.
     * @param sector_count Number of sectors to transfer.
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *         If `buffer` is a `nullptr`.
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
    [[nodiscard]]
    status_t 
    Programmable_IO::poll_and_read_or_write_disk(_INOUT_ uint16_t* buffer,
                                                 _IN_    const uint32_t sector_count) 
                                                 noexcept { 
        status_t status;

        if (!buffer) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_A;
            goto cleanup;
        }

        if (operation != Driver_Operation::READ && 
            operation != Driver_Operation::WRITE) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_B;
            goto cleanup;
        }

        if (sector_count == 0) [[unlikely]] {
            status = status::ATA_INVALID_SECTOR_COUNT | status::flags::PARAM_C;
            goto cleanup;
        }

        for (uint32_t sector  = 0;
             sector < sector_count; 
             sector++) [[likely]] {
            delay();

            status = poll_until_drq_or_error();
            if (status != status::SUCCESS) [[unlikely]] {
                goto cleanup;
            }

            if (operation == Driver_Operation::READ) {
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


    /**
     * @brief Executes a complete ATA-PIO read or write operation starting at
     *        a specific LBA address.
     *
     * @param buffer       I/O buffer for sector data.
     * @param relative_lba Starting LBA address.
     * @param sector_count Number of sectors to transfer.
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *         If `buffer` is a `nullptr`.
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
    [[nodiscard]]
    status_t 
    Programmable_IO::start_pio_disk_read_or_write(_INOUT_ uint16_t* const buffer,
                                                  _IN_    const uint32_t relative_lba,
                                                  _IN_    const uint32_t sector_count) 
                                                  noexcept {
        status_t status;
        uint32_t absolute_lba;
        uint8_t final_status;

        status = validate_storage_access(buffer, relative_lba, sector_count);
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
                            ((absolute_lba >> 24) & NIBBLE_MASK) | lba_flags);
                           
        if (operation == Driver_Operation::READ) {
            stdlib::byte_output(status_port, READ_SECTORS);
        }
        else {
            stdlib::byte_output(status_port, WRITE_SECTORS);
        }

        status = poll_and_read_or_write_disk(buffer, sector_count);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        if (operation == Driver_Operation::WRITE) {
            stdlib::byte_output(status_port, FLUSH_CACHE);

            status = poll_until_not_bsy_or_error();
            if (status != status::SUCCESS) [[unlikely]] {
                goto cleanup;
            }
        }

        final_status = stdlib::byte_input(status_port);

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


    /**
     * @brief ATA-PIO main method performing chunked read/write
     *        operations over a specific sector range.
     *
     * @param buffer       I/O buffer for sector data
     * @param relative_lba Starting LBA address
     * @param sector_count Number of sectors to transfer
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *         If `buffer` is a `nullptr`.
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
    [[nodiscard]]
    status_t 
    Programmable_IO::run(_INOUT_ uint16_t* buffer,
                         _IN_    uint32_t relative_lba,
                         _IN_    uint32_t sector_count) noexcept {
        status_t status;
        uint32_t chunk;
        uint32_t eflags;
        bool transfer_ok;
        
        eflags = kernel::sys::save_eflags();
        kernel::sys::disable_interrupts();
        
        if (sector_count == 0) [[unlikely]] {
            sector_count = 256;
        }

        status = validate_storage_access(buffer, 
                                         relative_lba,
                                         sector_count);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        status = poll_until_not_bsy_or_error();
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

            transfer_ok = false;

            for (int32_t retry = MAX_RETRYS; retry >= 0; --retry) [[likely]] {
                status = start_pio_disk_read_or_write(buffer,
                                                      relative_lba,
                                                      chunk);

                if (status == status::SUCCESS) {
                    transfer_ok = true;
                    break;
                }

                reset_driver();
                delay();
            }

            if (!transfer_ok) [[unlikely]] {
                goto cleanup;
            }

            buffer       += chunk * SECTOR_WORD_SIZE;
            relative_lba += chunk;
            sector_count -= chunk;
        }

        status = status::SUCCESS;

    cleanup:
        reset_driver();

        kernel::sys::restore_eflags(eflags);

        return status;
    }


    /**
     * @brief Nulls the ATA-PIO driver.
     */
    _API_
    Programmable_IO::Programmable_IO() noexcept :
        partition_length(0),
        lba_start_address(0),
        device_control_register(0),
        io_port_base(0),
        drive_select_flags(0),
        lba_flags(0),
        status_port(0),
        operation(Driver_Operation::NONE) {
        kernel::sys::disable_interrupts();
        reset_driver();
        kernel::sys::enable_interrupts();
    }


    /**
     * @brief Initializes the ATA-PIO driver.
     */
    _API_
    void 
    Programmable_IO::init() noexcept {
        uint32_t eflags;
        struct Channel final { 
            uint16_t io_port; 
            uint16_t control_register; 
        };
        status_t status;

        eflags = kernel::sys::save_eflags();
        kernel::sys::disable_interrupts();

        const Channel channels[] = { 
            {
                IDE_PRIMARY_IO_BASE,   
                IDE_PRIMARY_DCR_BASE
            }, 
            {
                IDE_SECONDARY_IO_BASE, 
                IDE_SECONDARY_DCR_BASE
            } 
        };

        for (const auto& channel : channels) [[likely]] {
            status = probe_and_configure_channel(channel.io_port, 
                                                 channel.control_register);
            if (status == status::SUCCESS) {
                goto cleanup;
            }
        }

        kernel::sys::restore_eflags(eflags);
        kernel::sys::panic(
            "ATA init from 'IDENTIFY' failed, no device detected"
        );

    cleanup:
        kernel::sys::restore_eflags(eflags);
    }


    /**
     * @brief ATA-PIO interface performing chunked read
     *        operations over a specific sector range.
     *
     * @param buffer       I/O buffer for sector data
     * @param relative_lba Starting LBA address
     * @param sector_count Number of sectors to transfer
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *         If `buffer` is a `nullptr`.
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
    _API_
    [[nodiscard]]
    status_t
    Programmable_IO::load_sectors(_INOUT_ uint16_t* buffer,
                                  _IN_    uint32_t relative_lba,
                                  _IN_    uint32_t sector_count) noexcept {
        operation = Driver_Operation::READ;

        return run(buffer, relative_lba, sector_count);
    }


    /**
     * @brief ATA-PIO interface performing chunked write
     *        operations over a specific sector range.
     *
     * @param buffer       I/O buffer for sector data
     * @param relative_lba Starting LBA address
     * @param sector_count Number of sectors to transfer
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *         If `buffer` is a `nullptr`.
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
    _API_
    [[nodiscard]]
    status_t
    Programmable_IO::store_sectors(_INOUT_ uint16_t* buffer,
                                   _IN_    uint32_t relative_lba,
                                   _IN_    uint32_t sector_count) noexcept {
        operation = Driver_Operation::WRITE;

        return run(buffer, relative_lba, sector_count);
    }
} // namespace drivers::ata
