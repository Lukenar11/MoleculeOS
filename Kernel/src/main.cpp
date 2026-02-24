#include "IDT/IDT.hpp"
#include "tests/Terminal/terminal_vga_driver.hpp"

namespace kernel {

    extern "C" void main() noexcept {

        idt::IDT interrupt_descriptor_table;

        test_vga_driver();
    
        while (true) 
            __asm__ volatile("hlt");
    }
} // namespace kernel
