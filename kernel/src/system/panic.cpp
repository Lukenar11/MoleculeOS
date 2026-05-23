/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
    
DESCRIPTION:
    This file implements the kernel panic routine, which is used to
    report fatal system errors and halt execution in a controlled way.

    The panic function prints a formatted error message along with
    troubleshooting information before stopping the CPU via "halt()".

NOTES:
*/

#include "system/panic.hpp"

namespace kernel::system
{
    void panic(const char* error_message, 
               const char* troubleshooting_message) noexcept {
        runtime::text_output.reset();
        runtime::text_output.set_text_color(
            drivers::vga::VGATextmodeColors::RED,
            drivers::vga::VGATextmodeColors::BLACK
        );

        if (!error_message)
            error_message = "Unknown fatal error.";

        if (!troubleshooting_message)
            troubleshooting_message = "No troubleshooting information available.";

        runtime::text_output.put_string(">>>>>>>>>>> KERNEL PANIC <<<<<<<<<<<\n");
        runtime::text_output.put_string("\nError:\n");
        runtime::text_output.put_string(error_message);
        runtime::text_output.put_string("\n\n--------- Troubleshooting ---------\n\n");
        runtime::text_output.put_string(troubleshooting_message);
        runtime::text_output.put_string("\n\n>>>>>>>>>> SYSTEM HALTED <<<<<<<<<<\n");

        kernel_system_hang();
    }
}
