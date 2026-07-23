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
    which calls 'isr_handler' via the IDT.

NOTES:
*/

#include "sys/panic.hpp"

namespace kernel::sys
{
    void panic(const char* message) noexcept {
        idt::isr::exception_names[CUSTOM_PANIC_INTERRUPT] = const_cast<char*>(message);
        trigger_interrupt(CUSTOM_PANIC_INTERRUPT);
    }
} // namespace kernel::sys
