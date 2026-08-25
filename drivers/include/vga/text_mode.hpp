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

#pragma once

#include "../utils/vga_text_mode_helpers.hpp"
#include <types.hpp>
#include <sal.hpp>
#include <status.hpp>

namespace drivers::vga 
{
    class Text_Mode final {
    private:
        static inline constexpr uint8_t BLINK_MODE_BIT = 0x80;
        static inline volatile uint16_t* const SCREEN_BUFFER = 
            reinterpret_cast<volatile uint16_t*>(0xB8000);

    public:
        [[nodiscard]]
        static inline constexpr 
        uint8_t make_color(_IN_ const Text_Mode_Colors& foreground,
                           _IN_ const Text_Mode_Colors& background,
                           _IN_ const bool does_blink=false) noexcept {
            const uint8_t color = (static_cast<uint8_t>(background) << 4) | 
                                   static_cast<uint8_t>(foreground);

            return (does_blink) ? (color | BLINK_MODE_BIT) : color;
        }

        [[nodiscard]]
        static inline constexpr 
        uint16_t make_symbol_entry(_IN_ const char symbol, 
                                   _IN_ const uint8_t color) noexcept {
            return (static_cast<uint16_t>(color) << 8) | 
                    static_cast<uint16_t>(symbol);
        }

        static 
        status_t put_char_at(_IN_ const char symbol, 
                             _IN_ const uint8_t color, 
                             _IN_ const uint32_t x, 
                             _IN_ const uint32_t y) noexcept;

        static 
        void clear_screen(_IN_ const Text_Mode_Colors& color) noexcept;

        Text_Mode() noexcept  = default;
        ~Text_Mode() noexcept = default;
    };
} // namespace drivers::vga
