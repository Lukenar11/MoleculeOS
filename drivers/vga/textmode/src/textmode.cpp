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
*/

#include "textmode.hpp"

namespace drivers::vga 
{
    void Textmode::put_char_at(const char symbol, 
                               const uint8_t color, 
                               const uint32_t x, 
                               const uint32_t y) const noexcept {
        // VGA-Area Over/Underflow gurard
        if (x >= VGA_TEXMODE_SCREEN_WIDTH || 
            y >= VGA_TEXMODE_SCREEN_HEIGHT) [[unlikely]]
            return;

        const uint32_t index = static_cast<uint32_t>(y) * VGA_TEXMODE_SCREEN_WIDTH + 
                               static_cast<uint32_t>(x);
        VGA_TEXMODE_BUFFER[index] = make_symbol_entry(symbol, color);
    }

    void Textmode::clear_screen(const VGATextmodeColors& background) const noexcept {
        const uint8_t color = make_color(VGATextmodeColors::BLACK, background);
        const uint16_t entry = make_symbol_entry(' ', color);

        const uint32_t n = VGA_TEXMODE_SCREEN_WIDTH * VGA_TEXMODE_SCREEN_HEIGHT;
        for (uint32_t i = NULL; i < n; i++)
            VGA_TEXMODE_BUFFER[i] = entry;
    }

    // GLOBAL VGA-textmode object
    Textmode texmode;
} // namespace drivers::vga
