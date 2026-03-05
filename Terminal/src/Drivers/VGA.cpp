#include "Drivers/VGA.hpp"

namespace terminal::drivers {

    void VGA::put_char_at
        (const char symbol, const uint8_t color, const int32_t x, const int32_t y) 
        const noexcept {
            
            // VGA-Area Over/Underflow gurard
            if ((x < 0 || x >= VGA_WIDTH) || (y < 0 || y >= VGA_HEIGHT)) return;

            VGA_BUFFER[y * VGA_WIDTH + x] = make_entry(symbol, color);
    }

    void VGA::clear_screen(const VGAColors background) const noexcept {

        uint8_t color = make_color(VGAColors::BLACK, background);
        uint16_t entry = make_entry(' ', color);

        for (uint32_t y = 0; y < VGA_HEIGHT; y++)
            for (uint32_t x = 0; x < VGA_WIDTH; x++)
                VGA_BUFFER[y * VGA_WIDTH + x] = entry;
    }
} // namespace terminal::drivers
