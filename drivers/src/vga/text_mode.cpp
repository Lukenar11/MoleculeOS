/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This is a VGA text mode driver, which provides a
    low‑level interface for writing characters and colors directly to
    the VGA text buffer located at physical address '0xB8000'.

    The driver offers utilities for constructing color attributes,
    composing character entries, and writing text to specific screen
    coordinates, as well as clearing the entire display.

NOTES:
    All writes must follow the VGA text mode layout of 80×25 characters.

    Some methods are placed in the header 
    because they are so small that the compiler can inline them.
*/

#include "../vga/text_mode.hpp"

namespace drivers::vga 
{
    status_t Text_Mode::put_char_at(_IN_ const char symbol, 
                                    _IN_ const uint8_t color, 
                                    _IN_ const uint32_t x, 
                                    _IN_ const uint32_t y) noexcept {
        status_t status;
        uint32_t index;

        if (x >= TEXT_MODE_SCREEN_WIDTH || 
            y >= TEXT_MODE_SCREEN_HEIGHT) [[unlikely]] {
            status = status::INVALID_PARAMETER;
            goto cleanup;
        }

        index = static_cast<uint32_t>(y) * TEXT_MODE_SCREEN_WIDTH + 
                static_cast<uint32_t>(x);
        SCREEN_BUFFER[index] = make_symbol_entry(symbol, color);

        status = status::SUCCESS;

    cleanup:
        return status;
    }

    void Text_Mode::clear_screen(_IN_ const Text_Mode_Colors& background) 
                                 noexcept {
        const uint8_t color  = make_color(Text_Mode_Colors::BLACK, background);
        const uint16_t entry = make_symbol_entry(' ', color);
        const uint32_t n     = TEXT_MODE_SCREEN_WIDTH * TEXT_MODE_SCREEN_HEIGHT;

        for (uint32_t i = 0; i < n; i++) [[likely]] {
            SCREEN_BUFFER[i] = entry;
        }
    }
} // namespace drivers::vga
