/*
    LICENSE:
        Copyright (c) 2026 Lukenar11 (Luke Matthes)
        MIT Licensed
        https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
    
    DESCRIPTION:
        This header declares the kernel shutdown routine, which attempts to
        power off the machine by issuing the ACPI S5 shutdown command.
    
    NOTES:
        There is no uniform way to implement a shutdown, 
        which is why the shutdown routine is emulator-specific 
        or works on real hardware but is extremely complex to implement.

        This routine is emulator-specific and only works on QEMU, Bochs and VirtualBox.
*/

#pragma once

#include "system/panic.hpp"
#include <stdint.h>
#include <port_io.h>

namespace
{
    struct TryShutdownValues {
        uint16_t port = 0x0000;
        uint16_t value = 0x0000;
        bool is_8bit_mode = false;
    };

    constexpr TryShutdownValues shutdown_try_values[] = {
        { .port=0x0604, .value=0x2000, .is_8bit_mode=false },   // QEMU ACPI shutdown
        { .port=0x00F4, .value=0x0000, .is_8bit_mode=true  },   // QEMU ISA exit
        { .port=0xB004, .value=0x2000, .is_8bit_mode=false },   // Bochs shutdown
        { .port=0x4004, .value=0x3400, .is_8bit_mode=false }    // VirtualBox shutdown
    };

    inline uint32_t save_eflags() noexcept {
        uint32_t flags;
        __asm__ volatile("pushf\n pop %0" : "=r"(flags));
        return flags;
    }

    inline void restore_eflags(uint32_t flags) noexcept {
        __asm__ volatile("push %0\n popf" :: "r"(flags));
    }
}

namespace kernel::system
{
    void shutdown() noexcept;
} // namespace kernel::system
