#pragma once

#include <Runtime/CPP/ConsoleIO.hpp>

namespace kernel::idt { 

    extern "C" void isr_common_handler();
} // namespace kernel::idt
