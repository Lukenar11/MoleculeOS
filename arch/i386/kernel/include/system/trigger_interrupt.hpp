#pragma once

#include <stdint.h>

namespace kernel::system
{
    extern "C" 
    void trigger_interrupt(uint32_t interrupt_number);
} // namespace kernel::system
