#include "Panic/kernel_panic.hpp"

extern "C" [[noreturn]]
void kernel_panic(const char* error_message, const char* troubleshooting_message) noexcept
{
    runtime::console.reset();
    runtime::console.set_char_colors(
        drivers::vga::VGAColors::RED,
        drivers::vga::VGAColors::BLACK
    );

    runtime::console.put_string(">>>>>>>>>>> KERNEL PANIC <<<<<<<<<<<\n\n");

    runtime::console.put_string("Error:\n\t");
    runtime::console.put_string(error_message);

    runtime::console.put_string("\n\nTroubleshooting:\n\t");
    runtime::console.put_string(troubleshooting_message);

    runtime::console.put_string("\n\n>>>>>>>>>>> SYSTEM HALTED <<<<<<<<<<\n");

    while (true)
        __asm__ volatile(
            "cli\n"
            "hlt\n"
        );
}
