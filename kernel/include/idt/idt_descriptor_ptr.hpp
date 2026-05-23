/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    Structure defining the IDT descriptor pointer (IDTR) used by the CPU.

    This descriptor specifies the base address and size of the Interrupt
    Descriptor Table (IDT) and is loaded using the 'lidt' instruction.

NOTES:
    The structure must match the exact memory layout defined by the Intel
    System Developer’s Manual. 
   
    It is marked as packed ("__attribute__((packed));") 
    to prevent the compiler from inserting padding bytes, 
    which would corrupt the IDTR and lead to immediate system failure 
    (typically a triple fault).
*/

#pragma once

#include <stdint.h>

namespace kernel::idt 
{
    struct IDTDescriptorPTR final {
        uint16_t limit;
        uint32_t base;
    } __attribute__((packed));
} // namespace kernel::idt 
