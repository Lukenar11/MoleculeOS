#pragma once

#include <Runtime/C/stdint.h>

namespace kernel::idt {

    // needed for "lidt[...]"
    struct IDTDescriptorPTR final {

        uint16_t limit;
        uint32_t base;
    } __attribute__((packed));
} // namespace kernel::idt 
