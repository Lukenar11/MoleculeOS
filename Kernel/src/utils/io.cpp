#include "utils/io.hpp"

namespace kernel::utils
{
    const uint8_t inb(const uint16_t port) noexcept {
        uint8_t result;
        __asm__ volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
        return result;
    }

    void outb(const uint16_t port, uint8_t value) noexcept {
        __asm__ volatile ("outb %0, %1" : : "a"(value), "Nd"(port));
    }

    const uint16_t inw(const uint16_t port) noexcept {
        uint16_t result;
        __asm__ volatile ("inw %1, %0" : "=a"(result) : "Nd"(port));
        return result;
    }

    const uint16_t outw(const uint16_t port, uint16_t value) noexcept {
        __asm__ volatile ("outw %0, %1" : : "a"(value), "Nd"(port));
        return value;
    }
} // namespace kernel::utils
