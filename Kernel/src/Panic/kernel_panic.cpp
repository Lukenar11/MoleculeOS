#include "Panic/kernel_panic.hpp"

extern "C"
void kernel_panic(const char* error_message, const char* troubleshooting_message) noexcept
{
    runtime::console.reset();
    runtime::console.set_char_colors(
        drivers::vga::VGAColors::RED,
        drivers::vga::VGAColors::BLACK
    );

    runtime::console.printf(
        "%s%s%s%s%s%s%s%s\n",
        "\n",
        "Error:\n\t",
        error_message,
        "\n",
        "\nTroubleshooting:\n\t",
        troubleshooting_message,
        "\n",
        "\n>>>>>>>>>>> SYSTEM HALTED <<<<<<<<<<\n"
    );

    HaltSystem();
}
