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
#include "cpu_flags.hpp"
#include <stdint.h>
#include <array.hpp>
#include <port_io.hpp>

namespace
{
    struct Shutdown_Command_Mapping final {
        uint16_t port = 0;
        uint16_t value = 0;
        bool is_8bit_mode = false;
    };

    constexpr Shutdown_Command_Mapping emulator_specific_shutdown_command_mappings[] = {
        { .port=0x0604, .value=0x2000, .is_8bit_mode=false },   // QEMU ACPI shutdown
        { .port=0x00F4, .value=0x0000, .is_8bit_mode=true  },   // QEMU ISA exit
        { .port=0xB004, .value=0x2000, .is_8bit_mode=false },   // Bochs shutdown
        { .port=0x4004, .value=0x3400, .is_8bit_mode=false }    // VirtualBox shutdown
    };
}

namespace kernel::system
{
    void shutdown() noexcept;
} // namespace kernel::system
