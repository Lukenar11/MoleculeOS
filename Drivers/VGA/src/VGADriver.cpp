#include "VGADriver.hpp"

namespace drivers::vga 
{
    void VGADriver::put_char_at(const char symbol, 
                                const uint8_t color, 
                                const int32_t x, 
                                const int32_t y) const noexcept {
        // VGA-Area Over/Underflow gurard
        if ((x < 0 || x >= VGA_WIDTH) || (y < 0 || y >= VGA_HEIGHT)) [[unlikely]]
            return;

        const uint32_t index = 
            static_cast<uint32_t>(y) * VGA_WIDTH + 
            static_cast<uint32_t>(x);
        VGA_BUFFER[index] = make_symbol_entry(symbol, color);
    }

    void VGADriver::clear_screen(const VGAColors& background) const noexcept {
        const uint8_t color = make_color(VGAColors::BLACK, background);
        const uint16_t entry = make_symbol_entry(' ', color);

        for (uint32_t i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++)
            VGA_BUFFER[i] = entry;
    }
    
    VGADriver vga_driver;
} // namespace drivers::vga
