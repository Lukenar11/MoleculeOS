#include "IDT/IDT.hpp"
#include "tests/Terminal/terminal_io.hpp"
#include "tests/Kernel/idt_errors.hpp"

namespace kernel {

    extern "C" void kernel_main() noexcept {

        idt::IDT idt;
        test_terminal();

        run_all_exception_tests();

        while (true) 
            __asm__ volatile ("hlt");
    }
} // namespace kernel
