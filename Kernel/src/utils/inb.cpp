#include "utils/inb.hpp"

namespace kernel::utils
{
    uint8_t inb(const uint16_t port) noexcept {
        uint8_t result;
        __asm__ volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
        return result;
    }
} // namespace kernel::utils
