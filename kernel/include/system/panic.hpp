/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This header declares the kernel panic routine, which is used to
    report fatal system errors and halt execution in a controlled way.

    The panic function prints a formatted error message along with
    troubleshooting information before stopping the CPU via "halt()".

NOTES:
*/

#pragma once

#include "system/hang.hpp"
#include <text_output.hpp>

namespace kernel::system
{
    void panic(const char* error_message, 
               const char* troubleshooting_message) noexcept;
} // namespace kernel::system
