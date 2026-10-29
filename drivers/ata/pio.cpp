/**
 * @brief Implements an driver for ATA-PIO storage access (PATA 40-Pin).
 *
 * @copyright Copyright (c) 2026 Lukenar11 (Luke Matthes)
 * @license:  https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
 * @author    Lukenar11 (Luke Matthes)
 *
 * @note The driver only supports 28-Bit LBA's, for the sake of simplicity.
 */


#include "pio.hpp"

#include <array.hpp>


namespace kernel::sys
{
    _C_API_ 
    uint32_t 
    save_eflags();

    _C_API_ 
    void disable_interrupts();

    _C_API_ 
    void 
    restore_eflags(uint32_t);

    void 
    panic(const char* message) noexcept;
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
    /**
     * @brief Defines driver action states.
     * 
     * @note This is an abstract representation of the 
     *       user requested driver operation and 
     *       has nothing to do with the ATA protocol.
     */
    enum class Programmable_IO::Driver_Action : uint8_t {
        NONE  = 0x00,
        READ  = 0x01,
        WRITE = 0x02
    };


    /**
     * @brief Defines the available ATA status codes.
     * 
     * @details The values ​​correspond to the status codes from the 
     *          ATA status register, defined by the ATA specification.
     */
    enum class Programmable_IO::ATA_Status : uint8_t {
        ERR  = 0x01,
        DRQ  = 0x08,
        DF   = 0x20,
        DRDY = 0x40,
        BSY  = 0x80
    };


    /**
     * @brief Defines the available ATA error codes.
     * 
     * @details The values ​​correspond to the error codes from the 
     *          ATA error register, defined by the ATA specification.
     */
    enum class Programmable_IO::ATA_Error : uint8_t {
        AMNF  = 0x01,
        TK0NF = 0x02,
        ABRT  = 0x04,
        MCR   = 0x08,
        IDNF  = 0x10,
        MC    = 0x20,
        UNC   = 0x40,
        BBK   = 0x80
    };


    /**
     * @brief Defines the available ATA command codes.
     * 
     * @details The values correspond to the command register codes
     *          defined by the ATA specification.
     */
    enum class Programmable_IO::ATA_Command : uint8_t {
        READ_SECTORS    = 0x20,
        WRITE_SECTORS   = 0x30,
        FLUSH_CACHE     = 0xE7,
        IDENTIFY_DEVICE = 0xEC        
    };


    /**
     * @brief Defines the available ATA control register flags.
     * 
     * @details The values correspond to the control register bits,
     *          defined by the ATA specification.
     */
    enum class Programmable_IO::ATA_Control : uint8_t {
        DCR_DEFAULT = 0x00,
        RST         = 0x04
    };


    /**
     * @brief Defines the available ATA I/O and control port base addresses.
     * 
     * @details The values correspond to the standard primary and secondary
     *          ATA channel base addresses for the data and control registers.
     */
    enum class Programmable_IO::ATA_Port : uint16_t {
        IDE_SECONDARY_IO_BASE  = 0x0170,
        IDE_PRIMARY_IO_BASE    = 0x01F0,
        IDE_SECONDARY_DCR_BASE = 0x0376,
        IDE_PRIMARY_DCR_BASE   = 0x03F6
    };


    /**
     * @brief Defines the available ATA device select values.
     * 
     * @details The values correspond to the Drive/Head register base values
     *          used to select the master or slave device and to prepare the
     *          Drive/Head register for ATA access.
     */
    enum class Programmable_IO::ATA_Device : uint8_t {
        MASTER_SELECT = 0xA0,
        SLAVE_SELECT  = 0xB0,
        MASTER        = 0xE0,
        SLAVE         = 0xF0
    };


    /**
     * @brief Checks whether a memory access operation is within the
     *        allowed LBA range.
     *
     * @param buffer       Pointer to a buffer for sector data.
     * @param buffer_size  Buffer byte size.
     * @param relative_lba LBA start address.
     * @param to_transfer  Number of sectors to read or write.
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *          If `buffer` is a `nullptr`.
     * 
     * @retval `status::BUFFER_TOO_SMALL | status::flags::PARAM_B`
     *          If `buffer_size` is too small for the requested transfer.
     *
     * @retval `status::ATA_INVALID_LBA | status::flags::PARAM_C`
     *          If `relative_lba` is outside the valid partition range or 
     *          the requested transfer range exceeds the partition length.
     *
     * @retval `status::ATA_INVALID_SECTOR_COUNT | status::flags::PARAM_D`
     *          If `to_transfer` is `0` or exceeds the 
     *          allowed maximum sectorcount.
     * 
     * @retval `status::ATA_NO_DEVICE`
     *          If no device is configured.
     *
     * @retval `status::SUCCESS`
     *          Default case.
     */
    [[nodiscard]] 
    status_t
    Programmable_IO::validate_transfer_request(_IN_ const uint16_t* const buffer,
                                               _IN_ const uint32_t buffer_size,
                                               _IN_ const uint32_t relative_lba,
                                               _IN_ const uint32_t to_transfer)
                                               noexcept {
        status_t status = status::SUCCESS;

        if (identify_data.partition_length == 0) [[unlikely]] {
            status = status::ATA_NO_DEVICE;
            goto cleanup;
        }

        if (!buffer) [[unlikely]] {
            status = status::NULL_POINTER | status::flags::PARAM_A;
            goto cleanup;
        }

        if (to_transfer == 0 ||
            to_transfer > identify_data.partition_length ||
            to_transfer > MAX_ALLOWED_SECTOR_COUNT) [[unlikely]] {
            status = status::ATA_INVALID_SECTOR_COUNT | status::flags::PARAM_D;
            goto cleanup;
        }

        if (to_transfer > (buffer_size / SECTOR_SIZE)) [[unlikely]] {
            status = status::BUFFER_TOO_SMALL | status::flags::PARAM_B;
            goto cleanup;
        }

        if (relative_lba >= identify_data.partition_length ||
            to_transfer > 
            (identify_data.partition_length - relative_lba)) [[unlikely]] {
            status = status::ATA_INVALID_LBA | status::flags::PARAM_C;
            goto cleanup;
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
            stdlib::byte_input(identify_data.status_port);
        }
    }


    /**
     * @brief Performs a software reset of the hard drive and waits
     *        until the controller is operational again.
     */
    void 
    Programmable_IO::reset_driver() noexcept {
        uint8_t ata_status = 0;

        stdlib::byte_output(identify_data.device_control_register, 
                            static_cast<uint8_t>(ATA_Control::RST));

        stdlib::byte_output(identify_data.device_control_register, 
                            static_cast<uint8_t>(ATA_Control::DCR_DEFAULT));

        delay();

        for (int32_t timeout = 5'000'000; timeout >= 0; --timeout) [[likely]] {
            ata_status = stdlib::byte_input(identify_data.status_port);

            if (!(ata_status & static_cast<uint8_t>(ATA_Status::BSY)) && 
                 (ata_status & static_cast<uint8_t>(ATA_Status::DRDY))) {
                break;
            }
        }
    }


    /**
     * @brief Issues the ATA command `IDENTIFY` for the 
     *        currently selected drive.
     *
     * @param drive_select Drive/Head register value, 
     *                     selecting master or slave.
     */
    void
    Programmable_IO::issue_identify_command(_IN_ uint8_t drive_select) 
                                            noexcept {
        stdlib::byte_output(identify_data.io_port_base + 6, 
                            drive_select);
        delay();

        stdlib::byte_output(identify_data.io_port_base + 7, 
                            static_cast<uint8_t>(ATA_Command::IDENTIFY_DEVICE));
        delay();
    };


    /**
     * @brief Tries to identify a single device on the 
     *        already configured ATA channel, 
     *        with retries and error handling.
     *
     * @param id_command_buffer Pointer to a buffer for the 
     *                          `IDENTIFY` data.
     * @param drive_select      Drive/Head register value,
     *                          selecting master or slave.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_A`
     *          If `id_command_buffer` is a `nullptr`.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
     *          If `drive_select` is invalid.
     *
     * @retval `status::ATA_ERROR`
     *          If an unrecoverable ATA error is reported.
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
    Programmable_IO::try_drive_identify(_OUT_ uint16_t* const id_command_buffer,
                                        _IN_  const uint8_t drive_select) 
                                        noexcept {
        status_t status        = status::SUCCESS;
        uint8_t error_register = 0;

        if (!id_command_buffer) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_A;
            goto cleanup;
        }

        if (drive_select != static_cast<uint8_t>(ATA_Device::MASTER_SELECT) &&
            drive_select != static_cast<uint8_t>(ATA_Device::SLAVE_SELECT)) 
            [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_B;
            goto cleanup;
        }

        reset_driver();
        issue_identify_command(drive_select);

        for (int32_t retries = MAX_RETRIES; 
             retries >= 0; 
             --retries) [[likely]] {
            status = wait_until_ready_or_error();
            switch (status) {
            case status::ATA_ERROR:
                error_register = stdlib::byte_input(identify_data.error_port);
                reset_driver();

                if (error_register & (static_cast<uint8_t>(ATA_Error::MC) |
                                      static_cast<uint8_t>(ATA_Error::MCR))) {
                    delay();
                    issue_identify_command(drive_select);
                    continue;
                }

                if (error_register & static_cast<uint8_t>(ATA_Error::TK0NF)) {
                    retries = 1;
                    delay();
                    issue_identify_command(drive_select);
                    continue;
                }
                break;

            case status::SUCCESS:
                stdlib::word_input_stream(identify_data.io_port_base, 
                                          SECTOR_WORD_SIZE, 
                                          id_command_buffer);
                break;

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
     * @brief Uses the ATA command `IDENTIFY` to retrieve the
     *        hard drive's identification data.
     *
     * @param id_command_buffer Pointer to a buffer for the 
     *                          `IDENTIFY` data.
     * @param drive_select      Drive/Head register value, 
     *                          selecting master or slave.
     * @param io_base           Base I/O port of the ATA channel.
     * @param control_register  Control register port of the ATA channel.
     * @param lba_mode          Drive/Head register base flags for the
     *                          LBA operation.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_A`
     *          If the pointer to `id_command_buffer` is `nullptr`.
     * 
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B` 
     *          If `drive_select` is invalid.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_C`
     *          If `io_base` is not a valid ATA channel.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_D`
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
    Programmable_IO::identify_drive(_OUT_ uint16_t* const id_command_buffer,
                                    _IN_  const uint8_t drive_select,
                                    _IN_  const uint16_t io_base,
                                    _IN_  const uint16_t control_register,
                                    _IN_  const uint8_t lba_mode)
                                    noexcept {
        status_t status = status::SUCCESS;

        if (!id_command_buffer) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_A;
            goto cleanup;
        }

        if (drive_select != static_cast<uint8_t>(ATA_Device::MASTER_SELECT) &&
            drive_select != static_cast<uint8_t>(ATA_Device::SLAVE_SELECT)) 
            [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_B;
            goto cleanup;
        }

        if (io_base != static_cast<uint16_t>(ATA_Port::IDE_PRIMARY_IO_BASE) &&
            io_base != static_cast<uint16_t>(ATA_Port::IDE_SECONDARY_IO_BASE)) 
            [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_C;
            goto cleanup;
        }

        if (control_register != 
            static_cast<uint16_t>(ATA_Port::IDE_PRIMARY_DCR_BASE) &&
            control_register != 
            static_cast<uint16_t>(ATA_Port::IDE_SECONDARY_DCR_BASE)
            ) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_D;
            goto cleanup;
        }

        identify_data.io_port_base            = io_base;
        identify_data.status_port             = identify_data.io_port_base + 7;
        identify_data.error_port              = identify_data.io_port_base + 1;
        identify_data.device_control_register = control_register;
        identify_data.drive_select_flags      = drive_select;
        identify_data.lba_flags               = lba_mode;

        status = try_drive_identify(id_command_buffer, drive_select);

        if (status == status::SUCCESS) [[likely]] {
            identify_data.lba_supported = id_command_buffer[49] & 
                                          ATA_LBA_SUPPORT_BIT;
        }

    cleanup:
        return status;
    }


    /**
     * @brief Probes an ATA channel (master + slave) and configures the 
     *        driver if a valid hard drive is detected.
     *
     * @param io_port          Base I/O port of the ATA channel.
     * @param control_register Control register port of the ATA channel.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_A`
     *          If `io_port` is not a valid ATA channel.
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
     *          If no channel is configured.
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

        struct Device_Config final {
            uint8_t drive_select = 0;
            uint8_t lba_mode     = 0;
        };

        stdlib::Array<uint16_t, SECTOR_WORD_SIZE> id_command_buffer;
        stdlib::Array<Device_Config, 2> devices;

        devices[0] = {
            static_cast<uint8_t>(ATA_Device::MASTER_SELECT), 
            static_cast<uint8_t>(ATA_Device::MASTER)
        };

        devices[1] = {
            static_cast<uint8_t>(ATA_Device::SLAVE_SELECT),  
            static_cast<uint8_t>(ATA_Device::SLAVE)
        };

        if (io_port != static_cast<uint16_t>(ATA_Port::IDE_PRIMARY_IO_BASE) &&
            io_port != static_cast<uint16_t>(ATA_Port::IDE_SECONDARY_IO_BASE)) 
            [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_A;
            goto cleanup;
        }

        if (control_register != 
            static_cast<uint16_t>(ATA_Port::IDE_PRIMARY_DCR_BASE) &&
            control_register != 
            static_cast<uint16_t>(ATA_Port::IDE_SECONDARY_DCR_BASE)) 
            [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_B;
            goto cleanup;
        }

        for (const auto& device : devices) [[likely]] {
            status = identify_drive(id_command_buffer.data(),
                                    device.drive_select,
                                    io_port,
                                    control_register,
                                    device.lba_mode);
            switch (status) {
            case status::INVALID_PARAMETER | status::flags::PARAM_A:
                status = status::FAIL;
                break;

            case status::INVALID_PARAMETER | status::flags::PARAM_B:
                status = status::FAIL;
                break;

            case status::INVALID_PARAMETER | status::flags::PARAM_C:
                // The status flag `PARAM_C` is changed to `PARAM_A`, 
                // so that the returned status code matches the 
                // method signature.

                status = status::INVALID_PARAMETER | status::flags::PARAM_A;
                break;

            case status::INVALID_PARAMETER | status::flags::PARAM_D:
                // The status flag `PARAM_D` is changed to `PARAM_B`, 
                // so that the returned status code matches the 
                // method signature.
                
                status = status::INVALID_PARAMETER | status::flags::PARAM_B;
                break;

            case status::SUCCESS:
                storage_size = (static_cast<uint32_t>(id_command_buffer[61]) << 16) |
                                static_cast<uint32_t>(id_command_buffer[60]);

                identify_data.io_port_base            = io_port;
                identify_data.device_control_register = control_register;
                identify_data.lba_start_address       = 0;

                if (storage_size != ATA_LBA28_OVERFLOW) [[likely]] {
                    identify_data.partition_length = storage_size;
                }
                else [[unlikely]] {
                    identify_data.partition_length = ATA_LBA28_MAX;
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
            ata_status = stdlib::byte_input(identify_data.status_port);

            if (ata_status & 
                static_cast<uint8_t>(ATA_Status::ERR)) [[unlikely]] {
                status = status::ATA_ERROR;
                goto cleanup;
            }

            if (ata_status & 
                static_cast<uint8_t>(ATA_Status::DF)) [[unlikely]] {
                status = status::ATA_DEVICE_FAULT;
                goto cleanup;
            }

            if (!(ata_status & static_cast<uint8_t>(ATA_Status::BSY)) && 
                 (ata_status & static_cast<uint8_t>(ATA_Status::DRQ))) {
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
            ata_status = stdlib::byte_input(identify_data.status_port);

            if (ata_status & 
                static_cast<uint8_t>(ATA_Status::ERR)) [[unlikely]] {
                status = status::ATA_ERROR;
                break;
            }

            if (ata_status & 
                static_cast<uint8_t>(ATA_Status::DF)) [[unlikely]] {
                status = status::ATA_DEVICE_FAULT;
                break;
            }

            if (!(ata_status & 
                static_cast<uint8_t>(ATA_Status::BSY))) {
                status = status::SUCCESS;
                break;
            }
        }

        return status;
    }


    /**
     * @brief Waits until the ATA status register indicates that a sector
     *        transfer can proceed.
     * 
     * @param buffer       Pointer to a buffer for the sector data.
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
    Programmable_IO::wait_until_sector_transfer_ready(_INOUT_ uint16_t* buffer,
                                                      _IN_    const uint32_t sector_count) 
                                                      noexcept { 
        status_t status = status::SUCCESS;

        if (!buffer) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_A;
            goto cleanup;
        }

        if (sector_count == 0) [[unlikely]] {
            status = status::ATA_INVALID_SECTOR_COUNT | status::flags::PARAM_B;
            goto cleanup;
        }

        for (int32_t retries = MAX_RETRIES; 
             retries >= 0; 
             --retries) [[likely]] {
            status = wait_until_ready_or_error();
            if (status != status::SUCCESS) [[unlikely]] {
                continue;
            }

            break;
        }

    cleanup:
        return status;
    }


    /**
     * @brief Performs a `PIO` read or write operation 
     *        for a given number of sectors.
     *
     * @param buffer       Pointer to a buffer for the sector data.
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
            status = status::ATA_INVALID_SECTOR_COUNT | status::flags::PARAM_B;
            goto cleanup;
        }

        for (uint32_t sector = 0; sector < sector_count; sector++) [[likely]] {
            delay();

            status = wait_until_sector_transfer_ready(buffer, sector_count);
            if (status != status::SUCCESS) [[unlikely]] {
                goto cleanup;
            }

            if (driver_action == Driver_Action::READ) {
                stdlib::word_input_stream(identify_data.io_port_base,
                                          SECTOR_WORD_SIZE,
                                          buffer);

            } 
            else {
                stdlib::word_output_stream(identify_data.io_port_base,
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
     * @param buffer       Pointer to a buffer for sector data.
     * @param buffer_size  Buffer byte size.
     * @param relative_lba LBA start address.
     * @param sector_count Number of sectors to transfer.
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *          If `buffer` is a `nullptr`.
     * 
     * @retval `status::BUFFER_TOO_SMALL | status::flags::PARAM_B` 
     *          If `buffer_size` is too small for the requested transfer.
     *
     * @retval `status::ATA_INVALID_LBA | status::flags::PARAM_C`
     *          If `relative_lba` is outside the valid partition range or 
     *          the requested transfer range exceeds the partition length.
     *
     * @retval `status::ATA_INVALID_SECTOR_COUNT | status::flags::PARAM_D`
     *          If `to_transfer` is `0` or exceeds the 
     *          allowed maximum sectorcount.
     * 
     * @retval `status::ATA_NO_DEVICE`
     *          If no device is configured.
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
    Programmable_IO::start_pio_transfer(_INOUT_ uint16_t* const buffer,
                                        _IN_    const uint32_t buffer_size,
                                        _IN_    const uint32_t relative_lba,
                                        _IN_    const uint32_t sector_count) 
                                        noexcept {
        status_t status       = status::SUCCESS;
        uint32_t absolute_lba = 0;

        status = validate_transfer_request(buffer, 
                                           buffer_size, 
                                           relative_lba, 
                                           sector_count);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        if (relative_lba > 
            (UINT32_MAX - identify_data.lba_start_address)) [[unlikely]] {
            status = status::ATA_INVALID_LBA | status::flags::PARAM_B;
            goto cleanup;
        }

        absolute_lba = relative_lba + identify_data.lba_start_address;

        stdlib::byte_output(identify_data.io_port_base + 2, 
                            static_cast<uint8_t>(sector_count));

        stdlib::byte_output(identify_data.io_port_base + 3, 
                            (absolute_lba >> 0) & BYTE_MASK);

        stdlib::byte_output(identify_data.io_port_base + 4, 
                            (absolute_lba >> 8) & BYTE_MASK);

        stdlib::byte_output(identify_data.io_port_base + 5, 
                            (absolute_lba >> 16) & BYTE_MASK);

        stdlib::byte_output(identify_data.io_port_base + 6,
                            ((absolute_lba >> 24) & NIBBLE_MASK) | 
                            identify_data.lba_flags);
                           
        if (driver_action == Driver_Action::READ) {
            stdlib::byte_output(identify_data.status_port, 
                                static_cast<uint8_t>(ATA_Command::READ_SECTORS));
        }
        else {
            stdlib::byte_output(identify_data.status_port, 
                                static_cast<uint8_t>(ATA_Command::WRITE_SECTORS));
        }

        status = transfer_sector_data(buffer, sector_count);
        switch (status) {
        case status::SUCCESS:
            break;

        case status::ATA_INVALID_SECTOR_COUNT | status::flags::PARAM_B:
            // The status flag `PARAM_B` is changed to `PARAM_D`, 
            // so that the returned status code matches the method signature.

            status = status::ATA_INVALID_SECTOR_COUNT | status::flags::PARAM_D;
            goto cleanup;

        default:
            goto cleanup;
        }

        if (driver_action == Driver_Action::WRITE) {
            stdlib::byte_output(identify_data.status_port, 
                                static_cast<uint8_t>(ATA_Command::FLUSH_CACHE));

            status = wait_until_unbusy_or_error();
        }

    cleanup:
        return status;
    }


    /**
     * @brief Tries to transfer one sector chunk, 
     *        with error and retries handling.
     *
     * @param buffer       Pointer to a buffer for sector data.
     * @param buffer_size  Buffer byte size.
     * @param relative_lba LBA start address.
     * @param chunk        Number of sectors in this transfer chunk.
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *          If `buffer` is a `nullptr`.
     * 
     * @retval `status::BUFFER_TOO_SMALL | status::flags::PARAM_B` 
     *          If buffer_size is too small for the requested transfer.
     *
     * @retval `status::ATA_INVALID_LBA | status::flags::PARAM_C`
     *          If `relative_lba` is outside the valid partition range or 
     *          the requested transfer range exceeds the partition length.
     *
     * @retval `status::ATA_INVALID_SECTOR_COUNT | status::flags::PARAM_D`
     *          If `to_transfer` is `0` or exceeds the 
     *          allowed maximum sectorcount.
     * 
     * @retval `status::ATA_NO_DEVICE`
     *          If no device is configured.
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
    Programmable_IO::try_chunk_transfer(_INOUT_ uint16_t* buffer,
                                        _IN_    uint32_t buffer_size,
                                        _IN_    uint32_t relative_lba,
                                        _IN_    uint32_t chunk) noexcept {
        status_t status        = status::ATA_TIMEOUT;
        uint8_t error_register = 0;

        status = validate_transfer_request(buffer, 
                                           buffer_size, 
                                           relative_lba, 
                                           chunk);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        for (int32_t retries = MAX_RETRIES; 
             retries >= 0; 
             --retries) [[likely]] {
            status = start_pio_transfer(buffer, 
                                        buffer_size, 
                                        relative_lba, 
                                        chunk);

            switch (status) {
            case status::ATA_DEVICE_FAULT:
                reset_driver();
                delay();
                continue;

            case status::ATA_TIMEOUT:
                reset_driver();
                delay();
                continue;

            case status::ATA_ERROR:
                error_register = stdlib::byte_input(identify_data.error_port);
                delay();

                if (error_register & (static_cast<uint8_t>(ATA_Error::MC) | 
                                      static_cast<uint8_t>(ATA_Error::MCR))) {
                    reset_driver();
                    delay();
                    continue;
                }
                
                if (error_register & static_cast<uint8_t>(ATA_Error::TK0NF)) {
                    retries = 1;

                    reset_driver();
                    delay();
                    continue;
                }
                break;
            }
            
            break;
        }

    cleanup:
        return status;
    }


    /**
     * @brief ATA-PIO main method performing chunked read/write
     *        operations over a specific sector range.
     *
     * @param buffer       Pointer to a buffer for sector data.
     * @param buffer_size  Buffer byte size.
     * @param relative_lba LBA start address.
     * @param sector_count Number of sectors to transfer.
     * 
     * @note If `sector_count` is `0`, the `sector_count` is set to `256`.
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *          If `buffer` is a `nullptr`.
     *
     * @retval `status::BUFFER_TOO_SMALL | status::flags::PARAM_B`
     *          If `buffer_size` is too small for the requested transfer.
     * 
     * @retval `status::ATA_INVALID_LBA | status::flags::PARAM_C`
     *          If `relative_lba` is outside the valid partition range or 
     *          the requested transfer range exceeds the partition length.
     *
     * @retval `status::ATA_INVALID_SECTOR_COUNT | status::flags::PARAM_D`
     *          If `sector_count` exceeds the allowed maximum sectorcount.
     * 
     * @retval `status::ATA_NO_DEVICE`
     *          If no device is configured.
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
                                              _IN_    uint32_t buffer_size,
                                              _IN_    uint32_t relative_lba,
                                              _IN_    uint32_t sector_count)
                                              noexcept {
        status_t status           = status::SUCCESS;
        uint32_t chunk            = 0;
        uint32_t chunk_byte_size  = 0;
        uint32_t used_buffer_size = 0;
        const uint32_t eflags     = kernel::sys::save_eflags();

        kernel::sys::disable_interrupts();

        if (sector_count == 0) [[unlikely]] {
            sector_count = MAX_ALLOWED_SECTOR_COUNT;
        }

        status = validate_transfer_request(buffer,
                                           buffer_size,
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

            chunk_byte_size = chunk * SECTOR_SIZE;

            if (chunk_byte_size > 
                (buffer_size - used_buffer_size)) [[unlikely]] {
                status = status::BUFFER_TOO_SMALL | status::flags::PARAM_B;
                break;
            }

            if (reinterpret_cast<uint32_t>(buffer) > 
                (UINT32_MAX - chunk_byte_size)) [[unlikely]] {
                status = status::BUFFER_TOO_SMALL | status::flags::PARAM_B;
                break;
            }

            status = try_chunk_transfer(buffer,
                                        buffer_size - used_buffer_size,
                                        relative_lba,
                                        chunk);
            if (status != status::SUCCESS) [[unlikely]] {
                break;
            }

            used_buffer_size += chunk_byte_size;

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
     * @brief Initializes one ATA channel and 
     *        retries probing on transient errors.
     *
     * @param io_port          Base I/O port of the ATA channel.
     * @param control_register Control register port of the ATA channel.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_A`
     *          If `io_port` is not a valid ATA channel.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
     *          If the `control_register` port is invalid.
     *
     * @retval `status::ATA_NO_DEVICE`
     *          If no device is configured.
     *
     * @retval `status::ATA_ERROR`
     *          If probing fails with an unrecoverable ATA error.
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
    Programmable_IO::init_channel(_IN_ const uint16_t io_port,
                                  _IN_ const uint16_t control_register) 
                                  noexcept {
        status_t status = status::SUCCESS;

        if (io_port != static_cast<uint16_t>(ATA_Port::IDE_PRIMARY_IO_BASE) &&
            io_port != static_cast<uint16_t>(ATA_Port::IDE_SECONDARY_IO_BASE)) 
            [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_A;
            goto cleanup;
        }

        if (control_register != 
            static_cast<uint16_t>(ATA_Port::IDE_PRIMARY_DCR_BASE) &&
            control_register != 
            static_cast<uint16_t>(ATA_Port::IDE_SECONDARY_DCR_BASE)
            ) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_B;
            goto cleanup;
        }

        for (int32_t retries = MAX_RETRIES; 
             retries >= 0; 
             --retries) [[unlikely]] {
            status = probe_and_configure_channel(io_port, control_register);

            switch (status) {
            case status::ATA_ERROR:
                continue;

            case status::ATA_DEVICE_FAULT:
                continue;

            case status::ATA_TIMEOUT:
                continue;
            }

            break;
        }

    cleanup:
        return status;
    }


    /**
     * @brief Nulls the ATA-PIO driver.
     */
    _API_
    Programmable_IO::Programmable_IO() noexcept :
        driver_action(Driver_Action::NONE),
        identify_data({}) {
    }


    /**
     * @brief Initializes the ATA-PIO driver.
     * 
     * @retval `status::ATA_NO_DEVICE`
     *          If no device is found.
     *
     * @retval `status::ATA_ERROR`
     *          If initialisation fails with an unrecoverable ATA error.
     *
     * @retval `status::ATA_DEVICE_FAULT`
     *          If the device enters an error state.
     *
     * @retval `status::ATA_TIMEOUT`
     *          If no response is received in the timeout period.
     * 
     * @retval `status::FAIL`
     *          Internal driver error.
     *
     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_
    [[nodiscard]]
    status_t 
    Programmable_IO::init() noexcept {
        status_t status = status::SUCCESS;
        const uint32_t eflags = kernel::sys::save_eflags();

        struct Channel final { 
            uint16_t io_port          = 0;
            uint16_t control_register = 0;
        };

        stdlib::Array<Channel, 2> channels;

        channels[0] = {
            static_cast<uint16_t>(ATA_Port::IDE_PRIMARY_IO_BASE),   
            static_cast<uint16_t>(ATA_Port::IDE_PRIMARY_DCR_BASE)
        };

        channels[1] = {
            static_cast<uint16_t>(ATA_Port::IDE_SECONDARY_IO_BASE), 
            static_cast<uint16_t>(ATA_Port::IDE_SECONDARY_DCR_BASE)
        };

        kernel::sys::disable_interrupts();

        for (const auto& channel : channels) [[likely]] {
            status = init_channel(channel.io_port, channel.control_register);
            switch (status) {
            case status::INVALID_PARAMETER | status::flags::PARAM_A:
                status = status::FAIL;
                break;

            case status::INVALID_PARAMETER | status::flags::PARAM_B:
                status = status::FAIL;
                break;

            case status::SUCCESS:
                break;

            default:
                continue;
            }

            break;
        }

        kernel::sys::restore_eflags(eflags);

        if (status == status::SUCCESS && 
            !identify_data.lba_supported) [[unlikely]] {
            kernel::sys::panic("LBA-Addressing not supported!");
        }

        return status;
    }


    /**
     * @brief ATA-PIO main method performing chunked read
     *        operations over a specific sector range.
     *
     * @param operation Operation struct with buffer pointer,
     *                  start LBA and sector count.
     * 
     * @note If `operation.sector_count` is `0`, 
     *       the `operation.sector_count` isset to `256`.
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *          If `operation.buffer` is a `nullptr`.
     *
     * @retval `status::ATA_INVALID_SECTOR_COUNT | status::flags::PARAM_A`
     *          If `operation.sector_count` exceeds the 
     *          allowed maximum sectorcount.
     *
     * @retval `status::ATA_INVALID_LBA | status::flags::PARAM_A`
     *          If `operation.relative_lba` is outside the 
     *          valid partition range or the 
     *          requested transfer range exceeds the partition length.
     *
     * @retval `status::ATA_NO_DEVICE` 
     *          If no device is configured.
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

        status_t status = execute_transfer_request(operation.buffer, 
                                                   operation.buffer_size,
                                                   operation.relative_lba, 
                                                   operation.sector_count);
        switch (status) {
        case status::ATA_INVALID_LBA | status::flags::PARAM_B:
            // The status flag `PARAM_B` is changed to `PARAM_A`, 
            // so that the returned status code matches the method signature.

            status = status::ATA_INVALID_LBA | status::flags::PARAM_A;
            break;

        case status::ATA_INVALID_SECTOR_COUNT | status::flags::PARAM_C:
            // The status flag `PARAM_C` is changed to `PARAM_A`, 
            // so that the returned status code matches the method signature.

            status = status::ATA_INVALID_SECTOR_COUNT | status::flags::PARAM_A;
            break;
        }   

        return status;
    }


    /**
     * @brief ATA-PIO main method performing chunked write
     *        operations over a specific sector range.
     *
     * @param operation Operation struct with buffer pointer,
     *                  start LBA and sector count.
     * 
     * @note If `operation.sector_count` is `0`, 
     *       the `operation.sector_count` isset to `256`.
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *          If `operation.buffer` is a `nullptr`.
     *
     * @retval `status::ATA_INVALID_SECTOR_COUNT | status::flags::PARAM_A`
     *          If `operation.sector_count` exceeds the 
     *          allowed maximum sectorcount.
     *
     * @retval `status::ATA_INVALID_LBA | status::flags::PARAM_A`
     *          If `operation.relative_lba` is outside the 
     *          valid partition range or the 
     *          requested transfer range exceeds the partition length.
     *
     * @retval `status::ATA_NO_DEVICE` 
     *          If no device is configured.
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
    Programmable_IO::store_sectors(_INOUT_ Programmable_IO_Operation& operation) 
                                   noexcept {
        driver_action = Driver_Action::WRITE;

        status_t status = execute_transfer_request(operation.buffer, 
                                                   operation.buffer_size,
                                                   operation.relative_lba, 
                                                   operation.sector_count);
        switch (status) {
        case status::ATA_INVALID_LBA | status::flags::PARAM_B:
            // The status flag `PARAM_B` is changed to `PARAM_A`, 
            // so that the returned status code matches the method signature.

            status = status::ATA_INVALID_LBA | status::flags::PARAM_A;
            break;

        case status::ATA_INVALID_SECTOR_COUNT | status::flags::PARAM_C:
            // The status flag `PARAM_C` is changed to `PARAM_A`, 
            // so that the returned status code matches the method signature.

            status = status::ATA_INVALID_SECTOR_COUNT | status::flags::PARAM_A;
            break;
        }

        return status;
    }
} // namespace drivers::ata
