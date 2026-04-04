#include "IDT/ISR/isr_common_handler.hpp"

namespace kernel::idt {

    extern "C" void isr_common_handler() {

        runtime::ConsoleIO console;
        console.reset();
        console.set_char_colors(
            drivers::vga::VGAColors::RED, 
            drivers::vga::VGAColors::BLACK
        );
        console.put_string("ERROR!");

        while (true)
            __asm__ volatile ("hlt");
    }
} // namespace kernel::idt
