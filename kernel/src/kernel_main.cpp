/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file contains the kernel entry point 'kernel_main', which
    is called directly after the bootloader hands control over to the kernel.

    The function initializes core subsyss such as the 'IDT' with the
    function 'kernel_arch_init", performs a simple interrupt- and 
    endless-loop-driven scheduling, and starts the shell.

NOTES:
    This 'kernel_main' function must never return. 
    If execution reaches the end of 'kernel_main', 
    a kernel panic is triggered to prevent undefined behavior.
*/

#include <kernel.hpp>
#include <terminal.hpp>
#include <drivers.hpp>

namespace kernel
{
    extern "C" [[noreturn]]
    void kernel_main() noexcept {
        kernel_arch_init();

        heap::Block_Allocator::init(&heap::heap_start, &heap::heap_end);
        drivers::ata::Programmable_Input_Output::init();
        storagemgr::Storage_Manager::init();

        if (!storagemgr::Storage_Manager::load_filesys())
            sys::panic("load failed");

        // schedule MoleculeOS
        static terminal::Terminal terminal;
        while (true) {
            sys::sleep();
            terminal.step();
            storagemgr::Storage_Manager::load_filesys();
        }

        if (!storagemgr::Storage_Manager::save_filesys())
            sys::panic("save failed");

        sys::panic("Unexpected return from the \"kernel_main\" scheduler main loop");
    }
}
