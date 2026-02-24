#pragma once

#include <stdint.h>

namespace terminal::drivers {

    inline volatile uint16_t* const VGA_BUFFER = (volatile uint16_t*)0xB8000;
    constexpr int VGA_WIDTH = 80;
    constexpr int VGA_HEIGHT = 25;

    // char/screen Colors
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

    // VGA-Driver
    class VGA {

        public:
            inline constexpr uint8_t make_color
                (const VGAColors foreground, const VGAColors background) 
                    const noexcept {return (uint8_t(background) << 4) | uint8_t(foreground);}

            inline constexpr uint16_t make_entry
                (const char symbol, const uint8_t color) 
                    const noexcept {return (uint16_t(color) << 8) | uint8_t(symbol);}

            inline void put_char_at
                (const char symbol, const uint8_t color, const int x, const int y) 
                    const noexcept {VGA_BUFFER[y * VGA_WIDTH + x] = make_entry(symbol, color);}

            void clear_screen(const uint8_t color) const noexcept;

            VGA() noexcept = default;
            ~VGA() noexcept = default;
    };

} // namespace terminal::drivers
