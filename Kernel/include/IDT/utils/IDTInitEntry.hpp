#pragma once

#include "IDT/ISR/isr.h"
#include "IDT/IRQ/irq.h"
#include <Runtime/C/stdint.h>

namespace kernel::idt 
{
    struct IDTInitEntry final {
        uint32_t index;
        void (*handler)();
    };

    static inline constexpr IDTInitEntry idt_init_table[0x6A] = {
        { 0, isr_0 }, { 1, isr_1 }, { 2, isr_2 }, { 3, isr_3 }, 
        { 4, isr_4 }, { 5, isr_5 }, { 6, isr_6 }, { 7, isr_7 }, 
        { 8, isr_8 }, { 9, isr_9 }, {10, isr_10}, {11, isr_11}, 
        {12, isr_12}, {13, isr_13}, {14, isr_14}, {15, isr_15}, 
        {16, isr_16}, {17, isr_17}, {18, isr_18}, {19, isr_19}, 
        {20, isr_20}, {21, isr_21}, {22, isr_22}, {23, isr_23}, 
        {24, isr_24}, {25, isr_25}, {26, isr_26}, {27, isr_27}, 
        {28, isr_28}, {29, isr_29}, {30, isr_30}, {31, isr_31},
        {0x69, isr_0x69},
        {32, irq_0 }, {33, irq_1 }, {34, irq_2 }, {35, irq_3 }, 
        {36, irq_4 }, {37, irq_5 }, {38, irq_6 }, {39, irq_7 }, 
        {40, irq_8 }, {41, irq_9 }, {42, irq_10}, {43, irq_11}, 
        {44, irq_12}, {45, irq_13}, {46, irq_14}, {47, irq_15}
    };
} // namespace kernel::idt 
