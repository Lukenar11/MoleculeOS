#include "IDT/IDT.hpp"
#include "tests/Runtime/test_console_io.hpp"
#include "tests/Kernel/idt_errors.hpp"
#include "Terminal/include/Terminal.hpp"

namespace kernel {

    extern "C" void kernel_main() {

        idt::IDT idt;

        test_console_io();

        while (true) 
            __asm__ volatile ("hlt");
    }
} // namespace kernel
