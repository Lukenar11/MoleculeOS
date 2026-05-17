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

        This routine is emulator-specific and only works on QEMU.
*/

#include "system/shutdown.hpp"

namespace kernel::system
{
    void shutdown() noexcept {
        const uint16_t qemu_acpi_power_management_port = 0x0604;
        const uint16_t slp_en_bit = 0x2000;
        outw(qemu_acpi_power_management_port, slp_en_bit);

        panic(
            "Shutdown failed", 
            "The shutdown methode is emulator spacific and works only on QEMU"
        );
    }
} // namespace kernel::system
