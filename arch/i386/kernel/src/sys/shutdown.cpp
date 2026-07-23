/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
    
DESCRIPTION:
    This file implements the kernel shutdown routine, which attempts to
    power off the machine by issuing the ACPI S5 shutdown command.
    
NOTES:
    There is no uniform way to implement a shutdown, 
    which is why the shutdown routine is emulator-specific 
    or works on real hardware but is extremely complex to implement.

    This routine is emulator-specific and only works on 
    'QEMU', 'Bochs' and 'VirtualBox'.
*/

#include "sys/shutdown.hpp"

namespace kernel::sys
{
    void shutdown() noexcept {
        const uint32_t cpu_flags = save_eflags();
        for (const auto& entry : emulator_specific_shutdown_command_mappings) {
            if (entry.is_8bit_mode) [[unlikely]] {
                const uint8_t value = entry.value & 0xFF;
                runtime::byte_output(entry.port, value);
            } 
            else [[likely]] {
                runtime::word_output(entry.port, entry.value);
            }

            restore_eflags(cpu_flags);
        }

        runtime::Text_Output::reset();

        const uint32_t line_count = sizeof(message_if_hardware_shutdown_not_success) / 
                                    sizeof(message_if_hardware_shutdown_not_success[0]);
        const uint32_t start_y = (drivers::vga::TEXT_MODE_SCREEN_HEIGHT - line_count) / 2;

        uint32_t length = 0;
        uint32_t x_pos  = 0;
        for (uint32_t i = 0; i < line_count; ++i) {
            length = runtime::String_Manipulation::get_string_length(
                         message_if_hardware_shutdown_not_success[i]
                     );
            x_pos = (drivers::vga::TEXT_MODE_SCREEN_WIDTH - length) / 2;

            runtime::Text_Output::set_cursor(x_pos, start_y + i);
            runtime::Text_Output::put_string(message_if_hardware_shutdown_not_success[i]);
        }

        sys::hang();
    }
} // namespace kernel::sys
