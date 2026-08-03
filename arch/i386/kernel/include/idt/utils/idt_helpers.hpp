/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file contains helper structures which 
    is needed for the 'IDT' initializing.

    'IDT_Descriptor_PTR' includes a from 'Intel' defined layout and 
    is used to load the 'IDT'.

    'IDT_Entry' is a from 'Intel' defined layout, 
    which represents all 'IDT' entry.

    'IDT_INIT_Entry' is a helpers structure, 
    which is used for the 'idt_init_table' to initialize the 'IDT'.

NOTES:
    The structures 'IDT_Descriptor_PTR' and 'IDT_Entry' has the 
    attribute '__attribute__((packed))', 
    which avoids byte padding because this structures must have a 
    exactly from 'Intel' defined byte layout.
*/

#pragma once

#include <types.h>

namespace kernel::idt 
{
    struct IDT_Descriptor_PTR final {
        uint16_t limit = 0;
        uint32_t base  = 0;
    } __attribute__((packed));

    struct IDT_Entry final {
        uint16_t base_low  = 0;
        uint16_t selector  = 0;
        uint8_t always_0   = 0;
        uint8_t flags      = 0;
        uint16_t base_high = 0;
    } __attribute__((packed));

    struct IDT_INIT_Entry final {
        uint8_t index = 0;
        void (*handler)();
    };
} // namespace kernel::idt 
