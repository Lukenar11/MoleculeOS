/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file contains the kernel entry point "kernel_main," which
    is called directly after the bootloader hands control over to the kernel.

    The function initializes core subsystems such as the IDT,
    performs a simple interrupt- and endless-loop-driven scheduling, and starts the shell.

NOTES:
    This "kernel_main" function must never return. If execution reaches the end of
    "kernel_main," a kernel panic is triggered to prevent undefined behavior.

    Since this function is called by an assembly routine,
    it is declared as "extern C" to ensure compatibility.
*/

#include "idt/idt.hpp"
#include "system/kernel_system_enable_interrupts.h"
#include "terminal.hpp"
#include "system/kernel_system_sleep.h"
#include "system/panic.hpp"
#include <stdint.h>

#include "tests/kernel/file_system.hpp"

void remap_pic() noexcept 
{
    struct PIC_Mapping {
        uint16_t port = 0;
        uint8_t value = 0;
    };

    const PIC_Mapping pic_mappings[] = {
        { .port=0x0020, .value=0x11 },  // init master IPC
        { .port=0x00A0, .value=0x11 },  // init slave IPC
        { .port=0x0021, .value=0x20 },  // set master interrupt vector offsets
        { .port=0x00A1, .value=0x28 },  // set slave interrupt vector offsets
        { .port=0x0021, .value=0x04 },  // Tell master where the slave is connected
        { .port=0x00A1, .value=0x02 },  // Tell slave its cascade identity
        { .port=0x0021, .value=0x01 },  // Set master 8086/88 mode
        { .port=0x00A1, .value=0x01 }   // Set slave 8086/88 mode
    };

    for (const auto& entry : pic_mappings) [[likely]]
        outb(entry.port, entry.value);
}

extern "C"
void kernel_main() 
{
    static kernel::idt::IDT idt;

    remap_pic();
    kernel_system_enable_interrupts();

    test_write_file_read_file();
    kernel_system_hang();

    // schedul MoleculeOS
    terminal::Terminal terminal;
    while (true) {
        kernel_system_sleep();
        terminal.step();
    }

    kernel::system::panic(
        "Unexpected return from the \"kernel_main\" scheduler main loop",
        "This should never happen.\nPlease report this to the developer."
    );
}
