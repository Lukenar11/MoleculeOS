/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
    
DESCRIPTION:
    This file implements the kernel panic routine, which is used to
    report fatal sys errors and halt execution in a controlled way.

    The panic function associates the char pointer it receives as a parameter 
    with an entry in the 'exception_names' table reserved for kernel panics;
    it then triggers an interrupt reserved for this purpose,
    which calls 'isr_common_handler' via the IDT.

NOTES:
*/

#pragma once

#include "isr_helpers.hpp"
#include "sys/interrupts.hpp"

namespace
{
    constexpr uint32_t CUSTOM_PANIC_INTERRUPT = 15;
}

namespace kernel::sys
{
    void panic(const char* message) noexcept;
} // namespace kernel::sys
