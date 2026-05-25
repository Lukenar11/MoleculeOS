/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    Class for creating and loading the Interrupt Descriptor Table (IDT). 

    This class reserves a 49-entry IDT,
    initializes all interrupt gates using the "IDTInitEntry" table and
    loads the final descriptor into the CPU using the "load_idt" function.

NOTES:
    The global 'idt' object is created in "kernel_main" 
    and not directly like other system components in the source file, 
    otherwise the compiler would remove it 
    since it does not see any direct usage related to other components.
*/

#include "idt/idt.hpp"

namespace kernel::idt 
{
    void IDT::set_gate(const uint8_t index, void (*handler)()) noexcept {
        if (index >= idt.size()) [[unlikely]]
            system::panic("IDT index out of range", "Check \"idt_init_table\"");
        
        const uint32_t base = reinterpret_cast<uint32_t>(handler);
        const uint16_t word_mask = 0xFFFF;

        IDT_Entry& idt_entry = idt[index];
        idt_entry.base_low = base & word_mask;
        idt_entry.base_high = (base >> 16) & word_mask;
        idt_entry.selector = CODE_SEGMENT_SELECTOR;
        idt_entry.always_0 = NULL;
        idt_entry.flags = FLAGS;
    }

    IDT::IDT() noexcept {
        // fill IDT-Descriptor
        idt_ptr.limit = (sizeof(IDT_Entry) * idt.size()) - 1;
        idt_ptr.base = reinterpret_cast<uintptr_t>(idt.begin());

        // clear table
        for (auto& idt_entry : idt) [[likely]]
            idt_entry = IDT_Entry{};

        // build IDT
        for (const auto& idt_entry : idt_init_table) [[likely]]
            set_gate(idt_entry.index, idt_entry.handler);

        load_idt(reinterpret_cast<uintptr_t>(&idt_ptr));
    }
} // namespache kernel::idt
