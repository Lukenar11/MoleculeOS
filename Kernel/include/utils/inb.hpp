#pragma once

#include <Runtime/C/stdint.h>

namespace kernel::utils
{
    uint8_t inb(const uint16_t port) noexcept;
} // namespace kernel::utils
