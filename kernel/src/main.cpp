/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file contains the kernel entry point 'kernel::main', which
    is called from 'startup_i386_system' after the 
    bootloader hands control over to the kernel.

    The function initializes core subsyss such as the 'IDT' with the
    function 'startup_i386_system', performs a simple interrupt- and 
    endless-loop-driven scheduling, and starts the shell.

NOTES:
    This 'kernel::main' function must never return. 
    If execution reaches the end of 'kernel::main', 
    a kernel panic is triggered to prevent undefined behavior.
*/


#include "main.hpp"
#include <kernel.hpp>
#include <status.hpp>
#include <terminal.hpp>
#include <drivers.hpp>


namespace kernel
{                
    _API_
    [[noreturn]]
    void 
    main() noexcept {
        heap::Block_Allocator::init(&heap::heap_start, &heap::heap_end);
        drivers::ata::Programmable_Input_Output::init();
        storemgr::Storage_Manager::init();

        if (storemgr::Storage_Manager::load_filesystem() != status::SUCCESS) [[unlikely]] {
            sys::panic("load failed");
        }
        
        // schedule MoleculeOS
        static terminal::Terminal terminal;
        while (true) {
            sys::sleep();
            terminal.step();
        }

        if (storemgr::Storage_Manager::save_filesystem() != status::SUCCESS) [[unlikely]] {
            sys::panic("save failed");
        }

        sys::panic("Unexpected return from the \"kernel_main\" scheduler main loop");
    }
}
