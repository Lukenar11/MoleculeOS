#pragma once

#include "isr/utils/exception_names.hpp"
#include "system/trigger_interrupt.hpp"

namespace kernel::system
{
    void panic(const char* message) noexcept;
} // namespace kernel::system
