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
#include "system/panic.hpp"

#include "tests/kernel/test_heap.hpp"

extern "C" 
void kernel_main() 
{
    static kernel::idt::IDT idt;
    
    shell::shell.run();

    kernel::system::panic(
        "Unexpected return from \'shell.run()\'",
        "This should never happen.\nPlease report this to the developer."
    );
}
