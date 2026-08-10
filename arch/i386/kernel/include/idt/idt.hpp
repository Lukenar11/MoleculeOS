/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    Class for creating and loading the 'Interrupt Descriptor Table' (IDT). 

    This class reserves a 256-entry 'IDT',
    initializes all interrupt gates using the 'IDT_INIT_Entry' table and
    loads the final descriptor into the CPU using the 'load_idt' function.

    'IDT_Descriptor_PTR' is used to have a pointer which loads the 'IDT'.

NOTES:
    The static 'idt' object is created in 'kernel_arch_init_arch_init' 
    and not directly like other system components in the source file, 
    otherwise the compiler would remove it 
    since it does not see any direct usage related to other components.
*/

#pragma once

#include "idt/utils/idt_helpers.hpp"
#include "idt/isr.hpp"
#include "idt/irq.hpp"
#include "load_idt.hpp"
#include <kernel.hpp>
#include <types.h>
#include <array.hpp>

namespace
{
    using namespace kernel::idt::isr;
    using namespace kernel::idt::irq;
    
    inline constexpr kernel::idt::IDT_INIT_Entry idt_init_table[] = {
        { 0, isr_0 }, { 1, isr_1 }, { 2, isr_2 },
        { 3, isr_3 }, { 4, isr_4 }, { 5, isr_5 },
        { 6, isr_6 }, { 7, isr_7 }, { 8, isr_8 },
        { 9, isr_9 }, {10, isr_10}, {11, isr_11},
        {12, isr_12}, {13, isr_13}, {14, isr_14},
        {15, isr_15}, {16, isr_16}, {17, isr_17},
        {18, isr_18}, {19, isr_19}, {20, isr_20},
        {21, isr_21}, {22, isr_22}, {23, isr_23},
        {24, isr_24}, {25, isr_25}, {26, isr_26},
        {27, isr_27}, {28, isr_28}, {29, isr_29},
        {30, isr_30}, {31, isr_31}, {32, irq_0 },
        {33, irq_1 }, {34, irq_2 }, {35, irq_3 },
        {36, irq_4 }, {37, irq_5 }, {38, irq_6 },
        {39, irq_7 }, {40, irq_8 }, {41, irq_9 },
        {42, irq_10}, {43, irq_11}, {44, irq_12},
        {45, irq_13}, {46, irq_14}, {47, irq_15}
    };
}

namespace kernel::idt 
{
    class Interrupt_Descriptor_Table final {
    private:
        static constexpr uint8_t FLAGS                 = 0x8E;
        static constexpr uint8_t ZERO_BYTE             = 0x00;
        static constexpr uint8_t CODE_SEGMENT_SELECTOR = 0x08;
        static constexpr uint16_t WORLD_MASK           = 0xFFFF;
        static constexpr uint32_t IDT_ENTRY_COUNT      = 256;

        runtime::Array<IDT_Entry, IDT_ENTRY_COUNT> idt;
        IDT_Descriptor_PTR idt_ptr;

        void set_gate(const uint8_t index, void (*handler)()) noexcept;

    public:
        Interrupt_Descriptor_Table() noexcept;
        ~Interrupt_Descriptor_Table() noexcept = default;
    };
} // namespace kernel::idt
