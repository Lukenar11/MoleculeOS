#pragma once

#include <Runtime/C/stdint.h>

namespace kernel::utils
{
    uint8_t inb(const uint16_t port) noexcept;
    void outb(const uint16_t port, uint8_t value) noexcept;
    
    uint16_t inw(const uint16_t port) noexcept;
    uint16_t outw(const uint16_t port, uint16_t value) noexcept;
} // namespace kernel::utils
