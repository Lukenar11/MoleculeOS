/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
    
DESCRIPTION:
    This is a VGA text mode driver, which provides a
    low‑level interface for writing characters and colors directly to
    the VGA text buffer located at physical address 0xB8000.
    
    The driver offers utilities for constructing color attributes,
    composing character entries, and writing text to specific screen
    coordinates, as well as clearing the entire display.
    
NOTES:
    All writes must follow the VGA text mode layout of 80×25 characters.

    Some methods are placed in the header 
    because they are so small that the compiler can inline them.
*/

#include "text_mode.hpp"

namespace drivers::vga 
{
    void Text_Mode::put_char_at(const char symbol, 
                                const uint8_t color, 
                                const uint32_t x, 
                                const uint32_t y) noexcept {
        // VGA-Area Over/Underflow gurard
        if (x >= TEXT_MODE_SCREEN_WIDTH || 
            y >= TEXT_MODE_SCREEN_HEIGHT) [[unlikely]]
            return;

        const uint32_t index = static_cast<uint32_t>(y) * TEXT_MODE_SCREEN_WIDTH +
                               static_cast<uint32_t>(x);
        VGA_TEXT_MODE_SCREEN_FRAME_BUFFER[index] = make_symbol_entry(symbol, color);
    }

    void Text_Mode::clear_screen(const Text_Mode_Colors& background) noexcept {
        const uint8_t color  = make_color(Text_Mode_Colors::BLACK, background);
        const uint16_t entry = make_symbol_entry(' ', color);

        const uint32_t n = TEXT_MODE_SCREEN_WIDTH * TEXT_MODE_SCREEN_HEIGHT;
        for (uint32_t i = 0; i < n; i++)
            VGA_TEXT_MODE_SCREEN_FRAME_BUFFER[i] = entry;
    }
} // namespace drivers::vga
