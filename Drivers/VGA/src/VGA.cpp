#include "VGA.hpp"

namespace drivers::vga {

    void VGA::put_char_at(
        const char symbol, 
        const uint8_t color, 
        const int32_t x, 
        const int32_t y) const noexcept {
            
            // VGA-Area Over/Underflow gurard
            if ((x < 0 || x >= types::VGA_WIDTH) || 
                (y < 0 || y >= types::VGA_HEIGHT)) [[unlikely]]
                return;

            types::VGA_BUFFER[y * types::VGA_WIDTH + x] = 
                make_symbol_entry(symbol, color);
    }

    void VGA::clear_screen(const types::VGAColors background) const noexcept {

        const uint8_t color = make_color(types::VGAColors::BLACK, background);
        const uint16_t entry = make_symbol_entry(' ', color);

        for (uint32_t y = 0; y < types::VGA_HEIGHT; y++)
            for (uint32_t x = 0; x < types::VGA_WIDTH; x++)
                types::VGA_BUFFER[y * types::VGA_WIDTH + x] = entry;
    }
} // namespace drivers::vga
