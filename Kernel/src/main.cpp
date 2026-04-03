#include "IDT/IDT.hpp"
#include "tests/Runtime/test_console_io.hpp"
#include "tests/Kernel/idt_errors.hpp"
#include "Terminal/include/Terminal.hpp"

namespace kernel {

    extern "C" void kernel_main() {

        idt::IDT idt;

        test_console_io();

        // volatile int a = 1;
        // volatile int b = 0;
        // volatile int c = a / b;  // This will trigger a Divide Error (ISR 0)
        
        while (true) 
            __asm__ volatile ("hlt");
    }
} // namespace kernel
