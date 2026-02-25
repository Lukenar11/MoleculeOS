#include "Drivers/VGA.hpp"

namespace terminal::drivers {

    constexpr uint8_t VGA::make_color(const VGAColors foreground, const VGAColors background)
        const noexcept {return (uint8_t(background) << 4) | uint8_t(foreground);}

    constexpr uint16_t VGA::make_entry(const char symbol, const uint8_t color) 
        const noexcept {return (uint16_t(color) << 8) | uint8_t(symbol);}

    void VGA::put_char_at
        (const char symbol, const uint8_t color, const int x, const int y) const noexcept {
            
            // VGA-Area Over/Underflow gurard
            if ((x < 0 || x >= VGA_WIDTH) || (y < 0 || y >= VGA_HEIGHT)) return;

            VGA_BUFFER[y * VGA_WIDTH + x] = make_entry(symbol, color);
    }

    void VGA::clear_screen(const uint8_t color) const noexcept {

        for (int y = 0; y < VGA_HEIGHT; y++)
            for (int x = 0; x < VGA_WIDTH; x++)
                put_char_at(
                    0x00, make_color(
                        VGAColors(color),
                        VGAColors(color)
                    ),
                    x, y
                );
    }
} // namespace terminal::drivers
