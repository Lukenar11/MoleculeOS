#pragma once

#include <Runtime/C/stdint.h>

namespace kernel::idt {
    
    #define IDT_NULL 0x00

    static constexpr uint8_t SHIFT_16 = 0x10;
    static constexpr uint16_t WORD_MASK = 0xFFFF;

    // IDT-Descriptor => IDTR-Register (LoadIDT.asm)
    extern "C" void LoadIDT(uint32_t);
} // namespace kernel::idt
