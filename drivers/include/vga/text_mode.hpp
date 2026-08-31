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
        static constexpr uint8_t BLINK_MODE_BIT = 0x80;

        static inline volatile uint16_t* 
        const SCREEN_BUFFER = reinterpret_cast<volatile uint16_t*>(0xB8000);


        /** 
         * @brief Makes a symbol entry for the 
         *        VGA-buffer from a specific color layout and a character.
         * 
         * @note Use `make_color()` to ensure the color layout is correct.
         * 
         * @param symbol symbol entry symbol
         * @param color  symbol entry color layout
         * 
         * @return symbol entry
         */
        _API_ [[nodiscard]] static inline constexpr uint16_t 
        make_symbol_entry(_IN_ const char symbol, 
                          _IN_ const uint8_t color) noexcept {
            return (static_cast<uint16_t>(color) << 8) | 
                    static_cast<uint16_t>(symbol);
        }


    public:
        /** 
         * @brief Makes a specific symbol color layout for the VGA-buffer.
         * 
         * @param foreground symbol color foreground
         * @param background symbol color background
         * @param does_blink symbol color blink mode (default case `false`)
         * 
         * @return symbol color layout
         */
        _API_ [[nodiscard]] static inline constexpr uint8_t
        make_color(_IN_ const Text_Mode_Colors& foreground,
                   _IN_ const Text_Mode_Colors& background,
                   _IN_ const bool does_blink=false) noexcept {
            const uint8_t color = (static_cast<uint8_t>(background) << 4) | 
                                   static_cast<uint8_t>(foreground);

            return (does_blink) ? (color | BLINK_MODE_BIT) : color;
        }


        /**
         * @brief Outputs a character with a special fore and background color.
         * 
         * @note The color must match a specific layout, 
         *       use `Text_Mode::make_color` 
         *       to ensure the color layout is correct.
         * 
         * @param x      X coordinate for the output
         * @param y      Y coordinate for the output
         * @param color  character color
         * @param symbol character output
         * 
         * @retval `status::INVALID_PARAMETER | status::flags::PARAM_A`
         *          If `x` is greater then the screen width.
         * 
         * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
         *          If `y` is less then the screen height.
         * 
         * @retval `status::success`
         *          Default Case.
         */
        _API_ static status_t 
        put_char_at(_IN_ const uint32_t x,
                    _IN_ const uint32_t y,
                    _IN_ const uint8_t color,
                    _IN_ const char symbol) noexcept;


        /** 
         * @brief Clears the screen and sets a special screen color.
         * 
         * @param background screen color
         */
        _API_ static void 
        clear_screen(_IN_ const Text_Mode_Colors& color) noexcept;


        Text_Mode() noexcept  = default;
        ~Text_Mode() noexcept = default;
    };
} // namespace drivers::vga
