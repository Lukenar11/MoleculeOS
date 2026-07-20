/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    Class for creating and loading the Interrupt Descriptor Table (IDT). 

    This class reserves a 256-entry IDT,
    initializes all interrupt gates using the "IDT_INIT_Entry" table and
    loads the final descriptor into the CPU using the "load_idt" function.

NOTES:
    The global "idt" object is created in "init_kernel" 
    and not directly like other sys components in the source file, 
    otherwise the compiler would remove it 
    since it does not see any direct usage related to other components.
*/

#pragma once

#include "idt_helpers.hpp"
#include "isr.hpp"
#include "irq.hpp"
#include "load_idt.hpp"
#include <kernel_api.hpp>
#include <stdint.h>
#include <array.hpp>

namespace
{
    inline constexpr kernel::idt::IDT_INIT_Entry idt_init_table[] = {
        { 0, kernel::isr::isr_0 }, { 1, kernel::isr::isr_1 }, { 2, kernel::isr::isr_2 },
        { 3, kernel::isr::isr_3 }, { 4, kernel::isr::isr_4 }, { 5, kernel::isr::isr_5 },
        { 6, kernel::isr::isr_6 }, { 7, kernel::isr::isr_7 }, { 8, kernel::isr::isr_8 },
        { 9, kernel::isr::isr_9 }, {10, kernel::isr::isr_10}, {11, kernel::isr::isr_11},
        {12, kernel::isr::isr_12}, {13, kernel::isr::isr_13}, {14, kernel::isr::isr_14},
        {15, kernel::isr::isr_15}, {16, kernel::isr::isr_16}, {17, kernel::isr::isr_17},
        {18, kernel::isr::isr_18}, {19, kernel::isr::isr_19}, {20, kernel::isr::isr_20},
        {21, kernel::isr::isr_21}, {22, kernel::isr::isr_22}, {23, kernel::isr::isr_23},
        {24, kernel::isr::isr_24}, {25, kernel::isr::isr_25}, {26, kernel::isr::isr_26},
        {27, kernel::isr::isr_27}, {28, kernel::isr::isr_28}, {29, kernel::isr::isr_29},
        {30, kernel::isr::isr_30}, {31, kernel::isr::isr_31}, {32, kernel::irq::irq_0 },
        {33, kernel::irq::irq_1 }, {34, kernel::irq::irq_2 }, {35, kernel::irq::irq_3 },
        {36, kernel::irq::irq_4 }, {37, kernel::irq::irq_5 }, {38, kernel::irq::irq_6 },
        {39, kernel::irq::irq_7 }, {40, kernel::irq::irq_8 }, {41, kernel::irq::irq_9 },
        {42, kernel::irq::irq_10}, {43, kernel::irq::irq_11}, {44, kernel::irq::irq_12},
        {45, kernel::irq::irq_13}, {46, kernel::irq::irq_14}, {47, kernel::irq::irq_15}
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
