/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file contains the internal implementation of 
    one of the terminal commands available in MoleculeOS.

    This command prints text to the screen or handle file streams.
    
    The commands can be called by the 
    shell interpreter from a table generated at compile time, 
    where each entry contains a specific hash and a function call to the 
    command's internal implementation.

NOTES:
*/

#pragma once

#include "../utils/cmds_helpers.hpp"
#include <types.hpp>
#include <array.hpp>
#include <string_manip.hpp>
#include <kernel.hpp>
#include <text_output.hpp>

namespace shell::commands
{
    void echo(const stdlib::Array<char, 64>& args) noexcept;
} // namespace shell::commands
