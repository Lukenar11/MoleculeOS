#pragma once

#include "helpers.hpp"
#include <Runtime/C/stdint.h>

/*
    Note:
    "__attribute__((packed))" prevents the compiler from filling the structure
    with padding bytes. 
    The IDT entries must exactly match the layout defined by
    Intel, as the CPU reads the fields directly from memory.
    Any deviation (e.g., due to alignment)
    would result in incorrect handler addresses or an immediate triple fault.
    Therefore, "packed" is absolutely necessary here.
*/

namespace kernel::idt 
{
    // Interrupt Descriptor (IDT) Entry structure
    struct IDTEntry final {
        uint16_t base_low;
        uint16_t selector;
        uint8_t always_0;
        uint8_t flags;
        uint16_t base_high;

        // _init_
        inline constexpr void set_gate(const uint32_t base, 
                                       const uint16_t selector, 
                                       const uint8_t flags) noexcept 
        {
            base_low = base & WORD_MASK;
            base_high = base >> SHIFT_16 & WORD_MASK;
            this->selector = selector;
            this->always_0 = IDT_NULL;
            this->flags = flags;
        }
    } __attribute__((packed));
} // namespace kernel::idt 
