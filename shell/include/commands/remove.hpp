/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file contains the internal implementation of 
    one of the terminal commands available in MoleculeOS.

    This command deletes a specific file.

    The commands can be called by the 
    shell interpreter from a table generated at compile time, 
    where each entry contains a specific hash and a function call to the 
    command's internal implementation.

NOTES:
*/

#pragma once

#include "utils/helpers.hpp"
#include "kernel/include/filesystem/mofs.hpp"
#include <array.hpp>
#include <text_output.hpp>

namespace shell::commands
{
    void remove(const runtime::Array<char, 64>& arguments) noexcept;
} // namespace shell::commands
