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

namespace kernel
{
    void remap_pic() noexcept {
        const PIC_Mapping pic_mappings[] = {
            { .port=0x0020, .value=0x11 },  // init master IPC
            { .port=0x00A0, .value=0x11 },  // init slave IPC
            { .port=0x0021, .value=0x20 },  // set master interrupt vector offsets
            { .port=0x00A1, .value=0x28 },  // set slave interrupt vector offsets
            { .port=0x0021, .value=0x04 },  // tell master where the slave is connected
            { .port=0x00A1, .value=0x02 },  // tell slave its cascade identity
            { .port=0x0021, .value=0x01 },  // set master 8086/88 mode
            { .port=0x00A1, .value=0x01 }   // set slave 8086/88 mode
        };

        for (const auto& entry : pic_mappings) [[likely]]
            outb(entry.port, entry.value);
    }

    void init_kernel() noexcept {
        static idt::Interrupt_Descriptor_Table idt;

        remap_pic();
        system::enable_interrupts();
    }
}
