#include "IDT/IDT.hpp"

namespace kernel::idt 
{
    IDT::IDT() noexcept {
        // fill IDT-Descriptor
        idt_ptr.limit = (sizeof(IDTEntry) * idt.size()) - 1;
        idt_ptr.base = reinterpret_cast<uintptr_t>(idt.begin());

        // Clear table
        for (uint32_t i = 0; i < idt.size(); i++)
            idt[i].set_gate(IDT_NULL, IDT_NULL, IDT_NULL);

        // _build_ IDT
        for (const auto& entry : idt_init_table)
            idt[entry.index].set_gate(
                reinterpret_cast<uintptr_t>(entry.handler),
                CODE_SEGMENT_SELECTOR, 
                FLAGS
            );

        LoadIDT(reinterpret_cast<uintptr_t>(&idt_ptr));
    }
} // namespache kernel::idt
