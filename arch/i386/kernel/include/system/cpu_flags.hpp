/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
	This file contains 2 simple assembly routines that 
    work using stack operations.

    The function "kernel_system_save_eflags" saves the 
    current CPU flags (flag register).

    The function "kernel_system_restore_eflags" restores the 
    CPU flags (flag register).

NOTES:
	The internal implementation is located in "cpu_flags.asm".
*/

#pragma once

#include <stdint.h>

namespace kernel::system
{
    extern "C" {
        uint32_t save_eflags(void);
        void restore_eflags(uint32_t param);
    }
} // namespace kernel::system
