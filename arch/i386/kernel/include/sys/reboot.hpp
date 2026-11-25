/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This is the kernel reboot routine, which triggers a
    hardware reset by issuing the appropriate command to the 
    keyboard controller (i8042).

NOTES:
*/

#pragma once

#include "panic.hpp"
#include <include/stdlib/types.hpp>
#include <include/drivers/ps2.hpp>
#include <include/stdlib/platform_io.hpp>

namespace kernel::sys
{
    void reboot() noexcept;
} // namespace kernel::sys
