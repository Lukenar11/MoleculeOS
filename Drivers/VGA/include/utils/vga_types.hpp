#pragma once

#include <Runtime/C/stdint.h>

namespace drivers::vga::types {

    // Screen Dimensions
    constexpr uint32_t VGA_WIDTH = 80;
    constexpr uint32_t VGA_HEIGHT = 25;

    // VGA Textmode-Buffer
    volatile uint16_t* const VGA_BUFFER =
        reinterpret_cast<volatile uint16_t*>(0xB8000);

    // Char/Symbol Color-Definition
    enum class VGAColors : uint8_t {

        BLACK = 0x00,
        BLUE = 0x01,
        GREEN = 0x02,
        CYAN = 0x03,
        RED = 0x04,
        MAGENTA = 0x05,
        BROWN = 0x06,
        LIGHT_GREY = 0x07,
        DARK_GREY = 0x08,
        LIGHT_BLUE = 0x09,
        LIGHT_GREEN = 0x0A,
        LIGHT_CYAN = 0x0B,
        LIGHT_RED = 0x0C,
        LIGHT_MAGENTA = 0x0D,
        YELLOW = 0x0E,
        WHITE = 0x0F
    };

} // namespace drivers::vga::types
