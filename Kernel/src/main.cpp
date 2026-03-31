#include "IDT/IDT.hpp"
#include "tests/Terminal/terminal_io.hpp"
#include "tests/Kernel/idt_errors.hpp"
#include "Terminal/include/Terminal.hpp"

namespace kernel {

    extern "C" void kernel_main() noexcept {

        idt::IDT idt;
        
        terminal::Terminal terminal;
        terminal.reset();
        terminal.put_string("Hello, World!\n");

        while (true) 
            __asm__ volatile ("hlt");
    }
} // namespace kernel
