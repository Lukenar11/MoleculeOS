/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    Definition of a single Interrupt Descriptor Table (IDT) entry.

    Each entry describes the CPU gate used to invoke an interrupt or
    exception handler. 

NOTES:
    This structure is marked as packed ("__attribute__((packed));") 
    to prevent the compiler from inserting padding bytes. 
    
    Any deviation from the required memory 
    layout would cause incorrect handler addresses to be loaded and
    may result in an immediate triple fault.
*/

#pragma once

#include <stdint.h>

namespace kernel::idt 
{
    // Interrupt Descriptor (IDT) Entry structure
    struct IDT_Entry final {
        uint16_t base_low = 0;
        uint16_t selector = 0;
        uint8_t always_0 = 0;
        uint8_t flags = 0;
        uint16_t base_high = 0;
    } __attribute__((packed));
} // namespace kernel::idt 
