/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    The function "init_kernel" is calles from "kernel_main" and 
    initializes core subsystems such as the IDT and remap the PIC.

NOTES:
*/

#include "arch/i386/kernel/include/init_kernel.hpp"

namespace kernel::arch
{
    void init_kernel() noexcept {
        static idt::Interrupt_Descriptor_Table idt;

        for (const auto& entry : pic_mappings) [[likely]]
            runtime::byte_output(entry.port, entry.value);

        system::enable_interrupts();
    }
} // namespace kernel::arch
