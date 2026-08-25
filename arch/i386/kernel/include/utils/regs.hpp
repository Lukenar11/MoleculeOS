/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This is the "register-dump structure"; which represents the
    exact CPU state saved by the ISR and IRQ assembly stubs before
    transferring control to the handlers.

NOTES:
    This structure is marked as packed ('__attribute__((packed))') 
    to prevent the compiler from inserting padding bytes. 
    
    The layout of this structure must match the stack layout created by
    the assembly routines; including segment registers; general-purpose
    registers; and exception metadata.
*/

#pragma once

#include <types.hpp>

namespace kernel
{
    extern "C"
    struct Registers final {
        uint32_t ds  = 0;
        uint32_t es  = 0;
        uint32_t fs  = 0;
        uint32_t gs  = 0;

        uint32_t edi = 0;
        uint32_t esi = 0;
        uint32_t ebp = 0;
        uint32_t ebx = 0;
        uint32_t edx = 0;
        uint32_t ecx = 0;
        uint32_t eax = 0;

        uint32_t interrupt_number = 0; 
        uint32_t error_code       = 0;

        uint32_t eip    = 0;
        uint32_t cs     = 0; 
        uint32_t eflags = 0;
    } __attribute__((packed));
} // namespace kernel
