/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file contains the internal implementation of 
    one of the terminal commands available in MoleculeOS.

    This command turns the System off.

    The commands can be called by the shell interpreter from a table generated at compile time, 
    where each entry contains a specific hash 
    and a function call to the command's internal implementation.

NOTES:
    Since this function is so small, 
    it is intentionally placed in the header so that the compiler can execute it inline.
*/

#pragma once

#include "kernel/include/system/shutdown.hpp"

namespace shell::commands
{
    inline void shutdown() noexcept { 
        kernel::system::shutdown(); 
    }
} // namespace shell::commands
