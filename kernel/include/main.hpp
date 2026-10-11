/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file contains the kernel entry point 'kernel::main', which
    is called from 'kernel::platform_bootstrap' after the 
    bootloader hands control over to the kernel.

    The function initializes core subsyss such as the 'IDT' with the
    function 'kernel::platform_bootstrap', performs a simple interrupt- and 
    endless-loop-driven scheduling, and starts the shell.

NOTES:
    This 'kernel::main' function must never return. 
    If execution reaches the end of 'kernel::main', 
    a kernel panic is triggered to prevent undefined behavior.
*/

#pragma once

#include <kernel.hpp>
#include <status.hpp>
#include <terminal.hpp>
#include <drivers.hpp>


namespace kernel
{                
    _API_
    [[noreturn]]
    void 
    main() noexcept;
}
