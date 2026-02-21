#include "IDT/IDT.hpp"
#include "tests/Kernel/idt_errors.hpp"
#include "tests/Atom/test_array.hpp"

namespace kernel {

    extern "C" void main() noexcept {

        volatile char* vga = (char*)0xB8820;
        const char* text_message = "Hello from C++ kernel!";

        for (int i = 0; text_message[i] != '\0'; i++) {

            vga[i * 2] = text_message[i];   // putc(text_message[i]);
            vga[(i * 2) + 1] = 0x02;        // Color: green
        }

        idt::IDT interrupt_descriptor_table;
        // run_all_exception_tests();
        test_array();

        while (true) 
            __asm__ volatile("hlt");
    }
} // namespace kernel
