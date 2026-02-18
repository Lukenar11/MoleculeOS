#include "IDT/IDT.hpp"
#include "idt_errors.hpp"

namespace kernel {

    extern "C" void main() {

        volatile char* vga = (char*)0xB8820;
        const char* text_message = "Hello from C++ kernel!";

        for (int i = 0; text_message[i] != '\0'; i++) {

            vga[i * 2] = text_message[i];   // putc(text_message[i]);
            vga[(i * 2) + 1] = 0x02;        // Color: green
        }

        idt::IDT interrupt_descriptor_table;
        run_all_exception_tests();
    }
} // namespace kernel
