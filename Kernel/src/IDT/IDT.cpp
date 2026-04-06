#include "IDT/IDT.hpp"

namespace kernel::idt {

    // _construct_
    IDT::IDT() noexcept {

        // fill IDT-Descriptor
        idt_ptr.limit = (sizeof(IDTEntry) * idt.size()) - 1;
        idt_ptr.base = reinterpret_cast<uint32_t>(idt.begin());

        // Clear table
        for (uint32_t i = 0; i < idt.size(); i++) [[likely]]
            idt[i].set_gate(IDT_NULL, IDT_NULL, IDT_NULL);

        // _build_ IDT
        for (const auto& entry : idt_init_table) [[likely]]
            idt[entry.index].set_gate(
                reinterpret_cast<uint32_t>(entry.handler),
                CODE_SEGMENT_SELECTOR, 
                FLAGS
            );

        LoadIDT(reinterpret_cast<uint32_t>(&idt_ptr));
    }
} // kernel::idt
