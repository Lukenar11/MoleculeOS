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

#include "idt/utils/isr_helpers.hpp"

namespace kernel::idt::isr
{
    const char* exception_names[] = {
        "Division by Zero",
        "Debug",
        "Non-Maskable Interrupt",
        "Breakpoint",
        "Overflow",
        "Bound Range Exceeded",
        "Invalid Opcode",
        "Device Not Available",
        "Double Fault",
        "Coprocessor Segment Overrun",
        "Invalid TSS",
        "Segment Not Present",
        "Stack Fault",
        "General Protection Fault",
        "Page Fault",
        nullptr,
        "x87 Floating-Point Exception",
        "Alignment Check",
        "Machine Check",
        "SIMD Floating-Point Exception",
        "Virtualization Exception",
        "Control Protection Exception",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
        "Reserved",
    };
} // namespace kernel::isr
