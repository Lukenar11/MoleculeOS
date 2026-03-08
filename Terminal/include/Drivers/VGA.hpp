#pragma once

#include <Atom/C/stdint.h>

namespace terminal::drivers {

    constexpr uint32_t VGA_WIDTH = 80;
    constexpr uint32_t VGA_HEIGHT = 25;
    inline volatile uint16_t* const VGA_BUFFER = 
        reinterpret_cast<volatile uint16_t*>(0xB8000);

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
            static constexpr uint8_t make_color
                (const VGAColors foreground, const VGAColors background) noexcept {
                    
                return (
                    (static_cast<uint8_t>(background) << 4) | 
                     static_cast<uint8_t>(foreground)
                );
            }

            static constexpr uint16_t make_entry
                (const char symbol, const uint8_t color) noexcept {
                
                return (
                    (static_cast<uint16_t>(color) << 8) | 
                     static_cast<uint16_t>(symbol)
                );
            }

            void put_char_at
                (const char symbol, const uint8_t color, const int32_t x, const int32_t y) 
                    const noexcept;

            void clear_screen(const VGAColors color) const noexcept;

            VGA() noexcept = default;
            ~VGA() noexcept = default;
    };
} // namespace terminal::drivers
