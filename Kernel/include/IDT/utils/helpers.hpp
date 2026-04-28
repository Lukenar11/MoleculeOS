#pragma once

#include <Runtime/C/stdint.h>

namespace kernel::idt 
{
    #define IDT_NULL 0x00

    constexpr auto WORD_MASK = 0xFFFF;
    constexpr auto SHIFT_16 = 0x10;
} // namespace kernel::idt
