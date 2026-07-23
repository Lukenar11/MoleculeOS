/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This function is called from 'kernel_main' and 
    initializes all architecture specific components/systems
    such as the IDT and remap the PIC.

NOTES:
*/

#include "kernel_arch_init.hpp"

namespace kernel
{
    void kernel_arch_init() noexcept {
        static idt::Interrupt_Descriptor_Table idt;

        for (const auto& entry : pic_mappings)
            runtime::byte_output(entry.port, entry.value);

        sys::enable_interrupts();
    }
} // namespace kernel
