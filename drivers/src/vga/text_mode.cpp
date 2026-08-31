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
    /**
     * @brief Outputs a character with a special fore and background color.
     * 
     * @note The color must match a specific layout, 
     *       use `Text_Mode::make_color` to ensure the color layout is correct.
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
    _API_ status_t 
    Text_Mode::put_char_at(_IN_ const uint32_t x,
                           _IN_ const uint32_t y,
                           _IN_ const uint8_t color,
                           _IN_ const char symbol) noexcept {
        status_t status;
        uint32_t buffer_index;

        if (x >= TEXT_MODE_SCREEN_WIDTH) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_A;
            goto cleanup;
        }

        if (y >= TEXT_MODE_SCREEN_HEIGHT) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_B;
            goto cleanup;
        }

        buffer_index = y * TEXT_MODE_SCREEN_WIDTH + x;
        SCREEN_BUFFER[buffer_index] = make_symbol_entry(symbol, color);

        status = status::SUCCESS;

    cleanup:
        return status;
    }


    /** 
     * @brief Clears the screen and sets a special screen color.
     * 
     * @param background screen color
     */
    _API_ void 
    Text_Mode::clear_screen(_IN_ const Text_Mode_Colors& background) noexcept {
        const uint8_t color  = make_color(Text_Mode_Colors::BLACK, background);
        const uint16_t entry = make_symbol_entry(' ', color);

        const uint32_t n = TEXT_MODE_SCREEN_WIDTH * TEXT_MODE_SCREEN_HEIGHT;

        for (uint32_t i = 0; i < n; i++) [[likely]] {
            SCREEN_BUFFER[i] = entry;
        }
    }
} // namespace drivers::vga
