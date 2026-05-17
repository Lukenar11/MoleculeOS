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

        static const char* kernel_panic_text_output[] = {
            ">>>>>>>>>>> KERNEL PANIC <<<<<<<<<<<\n",
            "\nError:\n",
            "\n\n--------- Troubleshooting ---------\n\n",
            "\n\n>>>>>>>>>> SYSTEM HALTED <<<<<<<<<<\n"
        };

        runtime::text_output.put_string(kernel_panic_text_output[0]);
        runtime::text_output.put_string(kernel_panic_text_output[1]);
        runtime::text_output.put_string(error_message);
        runtime::text_output.put_string(kernel_panic_text_output[2]);
        runtime::text_output.put_string(troubleshooting_message);
        runtime::text_output.put_string(kernel_panic_text_output[3]);

        halt();
    }
}
