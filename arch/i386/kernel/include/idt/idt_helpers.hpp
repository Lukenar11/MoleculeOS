#pragma once

#include <stdint.h>

namespace kernel::idt 
{
    struct IDT_Descriptor_PTR {
        uint16_t limit = 0;
        uint32_t base  = 0;
    } __attribute__((packed));

    struct IDT_Entry final {
        uint16_t base_low  = 0;
        uint16_t selector  = 0;
        uint8_t always_0   = 0;
        uint8_t flags      = 0;
        uint16_t base_high = 0;
    } __attribute__((packed));

    struct IDT_INIT_Entry final {
        uint8_t index = 0;
        void (*handler)();
    };
} // namespace kernel::idt 
