#include "IDT/IDT.hpp"
#include "tests/Terminal/terminal_io.hpp"

namespace kernel {

    extern "C" void main() noexcept {

        test_terminal();
        while (true) 
            __asm__ volatile("hlt");
    }
} // namespace kernel
