/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This is the "RegisterDump structure", which represents the
    exact CPU state saved by the ISR and IRQ assembly stubs before
    transferring control to the C-level handlers.

NOTES:
    This structure is marked as packed ("__attribute__((packed));") 
    to prevent the compiler from inserting padding bytes. 
    
    The layout of this structure must match the stack layout created by
    the assembly routines, including segment registers, general-purpose
    registers, and exception metadata.
*/

#pragma once

#include <stdint.h>

namespace kernel
{
    extern "C"
    struct Registers final {
        uint32_t ds  = 0, es  = 0, fs  = 0, gs  = 0;
        uint32_t edi = 0, esi = 0, ebp = 0, ebx = 0, edx = 0, ecx = 0, eax = 0;
        uint32_t interrupt_number = 0, error_code = 0;
        uint32_t eip = 0, cs = 0, eflags = 0;
    } __attribute__((packed));
} // namespace kernel
