/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    C++ interface for the common 'IRQ handler', 
    which is called by 'isr_stub'.

    This function receives a complete 'Registers' structure and
    performs uniform exception processing, including diagnostic output
    and system halt.

NOTES:
    Some functions are placed in the header 
    because they are so small that the compiler can inline them.
*/

#pragma once

#include "idt/utils/isr_helpers.hpp"
#include "utils/regs.hpp"
#include "sys/halt.hpp"
#include "sys/panic.hpp"
#include <text_output.hpp>

namespace
{
    inline constexpr void print_reg_dump(const char* label, 
                                         const uint32_t value) noexcept {
        stdlib::Text_Output::put_string(label, 
                                         /*clear_screen_if_hight_limit_reached=*/false);
        stdlib::Text_Output::put_hex(value);
    }
}

namespace kernel::idt::isr
{
    extern "C"
    void isr_handler(Registers* reg_dump);
} // namespace kernel::isr
