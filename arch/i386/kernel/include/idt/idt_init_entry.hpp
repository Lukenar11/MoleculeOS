/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    Initialization table for the Interrupt Descriptor Table (IDT).

    This file maps interrupt vector numbers to their corresponding
    ISR and IRQ handler entry points.

NOTES:
    This table is used during IDT construction to populate all
    interrupt gates with the correct handler addresses.

    The handlers themselves are implemented in Assembly and exposed
    through a C-compatible interface.
*/

#pragma once

#include "isr/isr.h"
#include "irq/irq.h"
#include <stdint.h>
#include <stddef.h>

namespace kernel::idt 
{
    struct IDT_INIT_Entry final {
        uint8_t index = 0;
        void (*handler)();
    };

    inline constexpr IDT_INIT_Entry idt_init_table[] = {
        { 0, isr::isr_0 }, { 1, isr::isr_1 }, { 2, isr::isr_2 },
        { 3, isr::isr_3 }, { 4, isr::isr_4 }, { 5, isr::isr_5 },
        { 6, isr::isr_6 }, { 7, isr::isr_7 }, { 8, isr::isr_8 },
        { 9, isr::isr_9 }, {10, isr::isr_10}, {11, isr::isr_11},
        {12, isr::isr_12}, {13, isr::isr_13}, {14, isr::isr_14},
        {15, isr::isr_15}, {16, isr::isr_16}, {17, isr::isr_17},
        {18, isr::isr_18}, {19, isr::isr_19}, {20, isr::isr_20},
        {21, isr::isr_21}, {22, isr::isr_22}, {23, isr::isr_23},
        {24, isr::isr_24}, {25, isr::isr_25}, {26, isr::isr_26},
        {27, isr::isr_27}, {28, isr::isr_28}, {29, isr::isr_29},
        {30, isr::isr_30}, {31, isr::isr_31}, {32, irq::irq_0 },
        {33, irq::irq_1 }, {34, irq::irq_2 }, {35, irq::irq_3 },
        {36, irq::irq_4 }, {37, irq::irq_5 }, {38, irq::irq_6 },
        {39, irq::irq_7 }, {40, irq::irq_8 }, {41, irq::irq_9 },
        {42, irq::irq_10}, {43, irq::irq_11}, {44, irq::irq_12},
        {45, irq::irq_13}, {46, irq::irq_14}, {47, irq::irq_15}
    };
} // namespace kernel::idt 
