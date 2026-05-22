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

        This routine is emulator-specific and only works on QEMU, Bochs and VirtualBox.
*/

#include "system/shutdown.hpp"

namespace kernel::system
{
    void shutdown() noexcept {
        const uint32_t cpu_flags = save_eflags();
        for (const auto& entry : shutdown_try_values) {
            if (entry.is_8bit_mode) [[unlikely]] {
                const uint16_t byte_mask = 0x00FF;

                const uint8_t port = entry.port & byte_mask;
                const uint8_t value = entry.value & byte_mask;

                outb(port, value);
            } else {
                const uint16_t port = entry.port;
                const uint16_t value = entry.value;

                outw(port, value);
            }

            restore_eflags(cpu_flags);
        }

        panic(
            "Shutdown failed",
            "No supported emulator-specific shutdown method worked.\n"
            "This system may not be running in a supported emulator."
        );
    }
} // namespace kernel::system
