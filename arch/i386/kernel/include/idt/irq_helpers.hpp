#pragma once

#include "utils/regs.hpp"
#include <stdint.h>

namespace kernel::idt::irq
{
    struct IRQ_Handler_Entry final {
        uint8_t index = 0;
        void (*handler)(kernel::Registers*);
    };
} // namespace kernel::irq
