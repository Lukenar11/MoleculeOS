/*
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

#include "drivers/ps2/keyboardin/include/utils/helpers.hpp"
#include "system/panic.hpp"
#include <stdint.h>
#include <io_arch_api.hpp>

namespace kernel::system
{
    void reboot() noexcept;
} // namespace kernel::system
