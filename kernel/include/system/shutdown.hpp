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

        This routine is emulator-specific and only works on QEMU.
*/

#pragma once

#include "system/panic.hpp"
#include <port_io.h>
#include <stdint.h>

namespace kernel::system
{
    void shutdown() noexcept;
} // namespace kernel::system
