/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    C/C++ interface for the common IRQ handler, 
    which is called by "isr_common_stub".

    This function receives a complete "RegisterDump" structure and
    performs uniform exception processing, including diagnostic output
    and system halt.

NOTES:
    Since this function is called by an assembly routine, 
    it is declared as "extern "C"" to ensure compatibility.

    The function "print_reg_dump" is deliberately only in the header 
    because it is quite small, so that the compiler can inline it, 
    since the compiler can better recognize and optimize headers.
*/

#pragma once

#include "utils/exception_names.hpp"
#include "arch/i386/kernel/include/utils/register_dump.hpp"
#include "system/hang.hpp"
#include <text_output.hpp>

static inline constexpr void print_reg_dump(const char* label, 
                                            const uint32_t value) noexcept
{
    runtime::text_output.put_string(label);
    runtime::text_output.put_hex(value);
}

namespace kernel::isr
{
    extern "C"
    void isr_common_handler(Register_Dump* reg_dump);
} // namespace kernel::isr
