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
    IDT::IDT() noexcept {
        // fill IDT-Descriptor
        idt_ptr.limit = (sizeof(IDTEntry) * idt.size()) - 1;
        idt_ptr.base = reinterpret_cast<uintptr_t>(idt.begin());

        // Clear table
        for (uint32_t i = NULL; i < idt.size(); i++)
            idt[i].set_gate(NULL, NULL, NULL);

        // _build_ IDT
        for (const auto& entry : idt_init_table)
            idt[entry.index].set_gate(
                reinterpret_cast<uintptr_t>(entry.handler),                     
                CODE_SEGMENT_SELECTOR,   
                FLAGS
            );

        load_idt(reinterpret_cast<uintptr_t>(&idt_ptr));
    }
} // namespache kernel::idt
