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

#pragma once

#include "utils/helpers.hpp"
#include <stdint.h>

namespace drivers::vga 
{
    class Text_Mode final {
    private:
        static constexpr uint8_t BLINK_MODE_BIT = 0x80;
        static inline volatile uint16_t* const VGA_TEXT_MODE_SCREEN_FRAME_BUFFER =
            reinterpret_cast<volatile uint16_t*>(0xB8000);

    public:
        [[nodiscard]]
        static inline constexpr uint8_t make_color(const Text_Mode_Colors& foreground,
                                                   const Text_Mode_Colors& background,
                                                   const bool does_blink=false) 
                                                   noexcept {
            const uint8_t color = (static_cast<uint8_t>(background) << 4) | 
                                   static_cast<uint8_t>(foreground);
            if (!does_blink) [[likely]]
                return color;
            else [[unlikely]]
                return color | BLINK_MODE_BIT;
        }

        [[nodiscard]]
        static inline constexpr uint16_t make_symbol_entry(const char symbol, 
                                                           const uint8_t color)
                                                           noexcept {
            return (static_cast<uint16_t>(color) << 8) | 
                    static_cast<uint16_t>(symbol);
        }

        void put_char_at(const char symbol, 
                         const uint8_t color, 
                         const uint32_t x, 
                         const uint32_t y) const noexcept;

        void clear_screen(const Text_Mode_Colors& color) const noexcept;

        Text_Mode() noexcept = default;
        ~Text_Mode() noexcept = default;
    };

    extern Text_Mode text_mode;
} // namespace drivers::vga
