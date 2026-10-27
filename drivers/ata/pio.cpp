/**
 * @brief Implements an driver for ATA-PIO storage access (PATA 40-Pin).
 *
 * @copyright Copyright (c) 2026 Lukenar11 (Luke Matthes)
 * @license:  https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
 * @author    Lukenar11 (Luke Matthes)
 *
 * @note The driver only supports LBA28, for the sake of simplicity.
 */


#include "pio.hpp"
#include <array.hpp>


namespace kernel::sys
{
    _C_API_
    uint32_t
    save_eflags();

    _C_API_
    void
    disable_interrupts();

    _C_API_
    void
    restore_eflags(uint32_t);
} // namespace kernel::sys


namespace stdlib
{
    _C_API_
    uint8_t 
    byte_input(uint16_t port);

    _C_API_
    void 
    word_input_stream(uint16_t port, 
                      uint32_t word_count,
                      uint16_t* buffer);

    _C_API_ 
    void
    byte_output(uint16_t port, uint8_t value);

    _C_API_
    void 
    word_output_stream(uint16_t port, 
                       uint32_t word_count,
                       uint16_t* buffer);
} // namespace stdlib


namespace drivers::ata
{
    enum class Programmable_IO::Driver_Action : uint8_t {
        NONE  = 0x00,
        READ  = 0x01,
        WRITE = 0x02
    };


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
    Programmable_IO::validate_transfer_request(_IN_ const uint16_t* const buffer,
                                               _IN_ const uint32_t relative_lba,
                                               _IN_ const uint32_t to_transfer)
                                               noexcept {
        status_t status = status::SUCCESS;

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
        }

    cleanup:
        return status;
    }


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
        uint8_t ata_status = 0;

        stdlib::byte_output(device_control_register, SRST);
        stdlib::byte_output(device_control_register, DCR_DEFAULT);

        delay();

        for (int32_t timeout = 5'000'000; timeout >= 0; --timeout) [[likely]] {
            ata_status = stdlib::byte_input(status_port);
            if (!(ata_status & ATA_BSY) && (ata_status & ATA_DRDY)) {
                break;
            }
        }
    }


    /**
     * @brief Issues the ATA command `IDENTIFY` for the 
     *        currently selected drive.
     *
     * @param drive_select Drive/Head register base value that selects the
     *                     target device.
     */
    void
    Programmable_IO::issue_identify_command(_IN_ uint8_t drive_select) 
                                            noexcept {
        stdlib::byte_output(io_port_base + 6, drive_select);
        delay();

        stdlib::byte_output(io_port_base + 7, ATA_IDENTIFY);
        delay();
    };


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
    Programmable_IO::identify_drive(_OUT_ uint16_t* const identify_data,
                                    _IN_  const uint16_t io_base,
                                    _IN_  const uint16_t control_register,
                                    _IN_  const uint8_t drive_select,
                                    _IN_  const uint8_t lba_mode)
                                    noexcept {
        status_t status   = status::SUCCESS;
        uint8_t ata_error = 0;

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
        issue_identify_command(drive_select);

        for (int32_t retries = MAX_RETRIES; 
             retries >= 0; 
             --retries) [[likely]] {
            status = wait_until_ready_or_error();
            switch (status) {
            case status::SUCCESS:
                stdlib::word_input_stream(io_base,
                                          SECTOR_WORD_SIZE, 
                                          identify_data);
                break;

            case status::ATA_ERROR:
                ata_error = stdlib::byte_input(io_port_base + 1);

                reset_driver();

                if ((ata_error & ATA_MC) || 
                    (ata_error & ATA_MCR)) {
                    delay();
                }

                if (ata_error & ATA_TK0NF) {
                    retries = 1;
                    delay();
                }

                if ((ata_error & ATA_BBK) ||
                    (ata_error & ATA_UNC) ||
                    (ata_error & ATA_IDNF) ||
                    (ata_error & ATA_AMNF) ||
                    (ata_error & ATA_ABRT)) {
                    break;
                }

                issue_identify_command(drive_select);
                continue;

            default:
                reset_driver();
                issue_identify_command(drive_select);
                continue;
            }

            break;
        }

    cleanup:
        return status;
    }


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
    Programmable_IO::probe_and_configure_channel(_IN_ const uint16_t io_port,
                                                 _IN_ const uint16_t control_register)
                                                 noexcept {
        status_t status       = status::ATA_NO_DEVICE;
        uint32_t storage_size = 0;

        stdlib::Array<uint16_t, SECTOR_WORD_SIZE> identify_data;

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
            switch (status) {
            case status::INVALID_PARAMETER | status::flags::PARAM_A:
                status = status::FAIL;
                break;

            case status::INVALID_PARAMETER | status::flags::PARAM_B:
                status = status::FAIL;
                break;

            case status::INVALID_PARAMETER | status::flags::PARAM_C:
                status = status::FAIL;
                break;

            case status::SUCCESS:
                storage_size = (static_cast<uint32_t>(identify_data[61]) << 16) |
                                static_cast<uint32_t>(identify_data[60]);

                io_port_base            = io_port;
                device_control_register = control_register;
                lba_start_address       = 0;

                if (storage_size != ATA_LBA28_OVERFLOW) [[likely]] {
                    partition_length = storage_size;
                }
                else [[unlikely]] {
                    partition_length = ATA_LBA28_MAX;
                }
                break;

            default:
                reset_driver();
                continue;
            }

            break;
        }

    cleanup:
        return status;
    }


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
    Programmable_IO::wait_until_ready_or_error() noexcept {
        status_t status    = status::ATA_TIMEOUT;
        uint8_t ata_status = 0;

        for (int32_t timeout = 5'000'000; timeout >= 0; --timeout) [[likely]] {
            ata_status = stdlib::byte_input(status_port);

            if (ata_status & ATA_ERR) [[unlikely]] {
                status = status::ATA_ERROR;
                goto cleanup;
            }

            if (ata_status & ATA_DF) [[unlikely]] {
                status = status::ATA_DEVICE_FAULT;
                goto cleanup;
            }

            if (!(ata_status & ATA_BSY) && (ata_status & ATA_DRQ)) {
                status = status::SUCCESS;
                goto cleanup;
            }
        }

    cleanup:
        return status;
    }


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
    Programmable_IO::wait_until_unbusy_or_error() noexcept {
        status_t status    = status::ATA_TIMEOUT;
        uint8_t ata_status = 0;

        for (int32_t timeout = 5'000'000; timeout >= 0; --timeout) [[likely]] {
            ata_status = stdlib::byte_input(status_port);

            if (ata_status & ATA_ERR) [[unlikely]] {
                status = status::ATA_ERROR;
                break;
            }

            if (ata_status & ATA_DF) [[unlikely]] {
                status = status::ATA_DEVICE_FAULT;
                break;
            }

            if (!(ata_status & ATA_BSY)) {
                status = status::SUCCESS;
                break;
            }
        }

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
    Programmable_IO::transfer_sector_data(_INOUT_ uint16_t* buffer,
                                          _IN_    const uint32_t sector_count) 
                                          noexcept { 
        status_t status = status::SUCCESS;

        if (!buffer) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_A;
            goto cleanup;
        }

        if (sector_count == 0) [[unlikely]] {
            status = status::ATA_INVALID_SECTOR_COUNT | status::flags::PARAM_C;
            goto cleanup;
        }

        for (uint32_t sector = 0; sector < sector_count; sector++) [[likely]] {
            delay();

            for (int32_t retries = MAX_RETRIES; 
                 retries >= 0; 
                 --retries) [[likely]] {
                status = wait_until_ready_or_error();

                switch (status) {
                case status::SUCCESS:
                    break;

                case status::ATA_TIMEOUT:
                    continue;

                default:
                    goto cleanup;
                }

                break;
            }

            if (status != status::SUCCESS) [[unlikely]] {
                goto cleanup;
            }

            if (driver_action == Driver_Action::READ) {
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

    cleanup:
        return status;
    }


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
    Programmable_IO::start_pio_transfer(_INOUT_ uint16_t* const buffer,
                                        _IN_    const uint32_t relative_lba,
                                        _IN_    const uint32_t sector_count) 
                                        noexcept {
        status_t status       = status::SUCCESS;
        uint32_t absolute_lba = 0;

        status = validate_transfer_request(buffer, relative_lba, sector_count);
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
                           
        if (driver_action == Driver_Action::READ) {
            stdlib::byte_output(status_port, READ_SECTORS);
        }
        else {
            stdlib::byte_output(status_port, WRITE_SECTORS);
        }

        status = transfer_sector_data(buffer, sector_count);
        if  (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        if (driver_action == Driver_Action::WRITE) {
            stdlib::byte_output(status_port, FLUSH_CACHE);

            status = wait_until_unbusy_or_error();
        }

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
    Programmable_IO::execute_transfer_request(_INOUT_ uint16_t* buffer,
                                              _IN_    uint32_t relative_lba,
                                              _IN_    uint32_t sector_count) 
                                              noexcept {
        status_t status   = status::SUCCESS;
        uint32_t chunk    = 0;
        uint32_t eflags   = 0;
        uint8_t ata_error = 0;
        bool transfer_ok  = false;
        
        eflags = kernel::sys::save_eflags();
        kernel::sys::disable_interrupts();
        
        if (sector_count == 0) [[unlikely]] {
            sector_count = MAX_ALLOWED_SECTOR_COUNT;
        }

        status = validate_transfer_request(buffer, 
                                           relative_lba,
                                           sector_count);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        status = wait_until_unbusy_or_error();
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

            for (int32_t retries = MAX_RETRIES; 
                 retries >= 0; 
                 --retries) [[likely]] {
                status = start_pio_transfer(buffer, relative_lba, chunk);
                switch (status) {
                case status::SUCCESS:
                    transfer_ok = true;
                    break;

                case status::ATA_ERROR:
                    ata_error = stdlib::byte_input(io_port_base + 1);
                    delay();

                    if (ata_error & (ATA_MC | ATA_MCR)) {
                        reset_driver();
                        delay();
                        continue;
                    }
                
                    if (ata_error & ATA_TK0NF) {
                        retries = 1;

                        reset_driver();
                        delay();
                        continue;
                    }
                
                    goto cleanup;

                case status::ATA_DEVICE_FAULT:
                    reset_driver();
                    delay();
                    continue;

                case status::ATA_TIMEOUT:
                    reset_driver();
                    delay();
                    continue;

                default:
                    goto cleanup;
                }

                if (transfer_ok) {
                    break;
                }
            }

            if (!transfer_ok) [[unlikely]] {
                goto cleanup;
            }
    
            buffer       += chunk * SECTOR_WORD_SIZE;
            relative_lba += chunk;
            sector_count -= chunk;
        }

    cleanup:
        reset_driver();
        delay();

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
        driver_action(Driver_Action::NONE) {
    }


    /**
     * @brief Initializes the ATA-PIO driver.
     */
    _API_
    status_t 
    Programmable_IO::init() noexcept {
        status_t status = status::SUCCESS;
        uint32_t eflags = kernel::sys::save_eflags();

        const struct Channel final { 
            uint16_t io_port          = 0;
            uint16_t control_register = 0;
        }
        channels[] = { 
            {
                IDE_PRIMARY_IO_BASE,   
                IDE_PRIMARY_DCR_BASE
            }, 
            {
                IDE_SECONDARY_IO_BASE, 
                IDE_SECONDARY_DCR_BASE
            } 
        };

        kernel::sys::disable_interrupts();

        for (const auto& channel : channels) [[likely]] {
            for (int32_t retries = MAX_RETRIES; 
                 retries >= 0; 
                 --retries) [[unlikely]] {
                status = probe_and_configure_channel(channel.io_port,
                                                     channel.control_register);
                switch (status) {
                case status::SUCCESS:
                    goto cleanup;

                case status::INVALID_PARAMETER | status::flags::PARAM_A:
                    status = status::FAIL;
                    goto cleanup;

                case status::INVALID_PARAMETER | status::flags::PARAM_B:
                    status = status::FAIL;
                    goto cleanup;

                case status::ATA_NO_DEVICE:
                    retries = 0;
                    break;

                default:
                    continue;
                }
            }
        }

    cleanup:
        kernel::sys::restore_eflags(eflags);

        return status;
    }


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
    Programmable_IO::load_sectors(_INOUT_ Programmable_IO_Operation& operation) 
                                  noexcept {
        driver_action = Driver_Action::READ;

        return execute_transfer_request(operation.buffer_ptr, 
                                        operation.relative_lba, 
                                        operation.sector_count);
    }


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
    Programmable_IO::store_sectors(_INOUT_ Programmable_IO_Operation& operation) 
                                   noexcept {
        driver_action = Driver_Action::WRITE;

        return execute_transfer_request(operation.buffer_ptr, 
                                        operation.relative_lba, 
                                        operation.sector_count);
    }
} // namespace drivers::ata
