/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT License
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This table is used by the "isr_common_handler" to display 
    meaningful diagnostic messages during exception reporting.

NOTES:
*/

#include "isr/utils/exception_names.hpp"

namespace kernel::isr
{
    char* exception_names[] = {
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
