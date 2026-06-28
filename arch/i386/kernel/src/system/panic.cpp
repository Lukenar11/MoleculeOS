/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
    
DESCRIPTION:
    This file implements the kernel panic routine, which is used to
    report fatal system errors and halt execution in a controlled way.

    The panic function associates the char pointer it receives as a parameter 
    with an entry in the 'exception_names' table reserved for kernel panics;
    it then triggers an interrupt reserved for this purpose,
    which calls 'isr_common_handler' via the IDT.

NOTES:
*/

#include "system/panic.hpp"

namespace kernel::system
{
    void panic(const char* message) noexcept {
        const uint32_t custom_panic_idt_interrupt = 15;

        isr::exception_names[custom_panic_idt_interrupt] = const_cast<char*>(message);
        trigger_interrupt(custom_panic_idt_interrupt);
    }
} // namespace kernel::system
