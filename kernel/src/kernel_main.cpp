/*
    LICENSE:
        Copyright (c) 2026 Lukenar11 (Luke Matthes)
        MIT Licensed
        https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
    
    DESCRIPTION:
        This file contains the kernel entry point "kernel_main", which is
        called directly after the bootloader transfers control to the kernel.

        The function initializes core subsystems such as the IDT, 
        and then launches the interactive shell as the primary user interface of the system.

    NOTES:
        This function must never return. If execution reaches the end of
        "kernel_main", a kernel panic is triggered to prevent undefined
        behavior.

        Since this function is called by an assembly routine, 
        it is declared as "extern "C"" to ensure compatibility.
*/

#include "idt/idt.hpp"
#include "shell.hpp"
#include "system/kernel_system_sleep.h"
#include "system/panic.hpp"

inline void remap_pic() noexcept 
{
    const uint16_t pic1_command = 0x20;
    const uint16_t pic1_data = 0x21;
    const uint16_t pic2_command = 0xA0;
    const uint16_t pic2_data = 0xA1;
    const uint8_t icw1_init = 0x11;
    const uint8_t icw4_8086 = 0x01;

    outb(pic1_command, icw1_init);
    outb(pic2_command, icw1_init);

    outb(pic1_data, 0x20);
    outb(pic2_data, 0x28);

    outb(pic1_data, 0x04);
    outb(pic2_data, 0x02);

    outb(pic1_data, icw4_8086);
    outb(pic2_data, icw4_8086);

    outb(pic1_data, 0xFD); // only IRQ1 unmasked on master
    outb(pic2_data, 0xFF); // all slave IRQs masked
}

extern "C" 
void kernel_main() 
{
    static kernel::idt::IDT idt;

    remap_pic();
    __asm__ volatile ("sti");

    // schedul MoleculeOS
    shell::Shell sh;
    while (true) {
        kernel_system_sleep();
        sh.step();
    }

    kernel::system::panic(
        "Unexpected return from the kernel main loop",
        "This should never happen.\nPlease report this to the developer."
    );
}
