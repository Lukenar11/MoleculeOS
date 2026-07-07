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
    and not directly like other system components in the source file, 
    otherwise the compiler would remove it 
    since it does not see any direct usage related to other components.
*/

#pragma once

#include "idt_entry.hpp"
#include "idt_descriptor_ptr.hpp"
#include "idt_init_entry.hpp"
#include "load_idt.hpp"
#include <kernel_api.hpp>
#include <stdint.h>
#include <array.hpp>

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
        IDT_Descriptor_PTR idt_ptr = nullptr;

        void set_gate(const uint8_t index, void (*handler)()) noexcept;

    public:
        Interrupt_Descriptor_Table() noexcept;
        ~Interrupt_Descriptor_Table() noexcept = default;
    };
} // namespace kernel::idt
