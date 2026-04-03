#pragma once

#include <Runtime/C/stdint.h>

namespace kernel::idt {
    
    #define IDT_NULL 0x00

    static constexpr uint32_t ENTRYS = 256;
    static constexpr uint32_t MASK = 0xFFFF;

    // IDT-Descriptor => IDTR-Register (LoadIDT.asm)
    extern "C" void LoadIDT(uint32_t);
} // namespace kernel::idt
