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
     * @brief Makes a symbol entry for the 
     *        VGA-buffer from a specific color layout and a character.
     * 
     * @note Use `make_symbol_color()` to ensure the color layout is correct.
     * 
     * @param symbol symbol entry symbol
     * @param color  symbol entry color layout
     * 
     * @return symbol entry
     */ 
    [[nodiscard]] 
    uint16_t 
    Text_Mode::make_symbol_entry(_IN_ const char symbol, 
                                 _IN_ const uint8_t color) noexcept {
        return (static_cast<uint16_t>(color) << COLOR_POSITION) | 
                static_cast<uint16_t>(symbol);
    }


    /**
     * @brief Initializes the VGA-textmode driver.
     */
    Text_Mode::Text_Mode() noexcept :
        screen_buffer_ptr(reinterpret_cast<volatile uint16_t*>(0xB8000)) {
    }


    /** 
     * @brief Clears the screen and sets a special screen color.
     * 
     * @param foreground screen text color (default case `BLACK`)
     * @param background screen color (default case `BLACK`)
     */
    _API_ 
    void 
    Text_Mode::clear_screen(_IN_ const Text_Mode_Color foreground,
                            _IN_ const Text_Mode_Color background) noexcept {
        const uint8_t color  = make_symbol_color(foreground, background);
        const uint16_t entry = make_symbol_entry(' ', color);

        for (uint32_t buffer_index = 0; 
             buffer_index < SCREEN_CHAR_WIDTH * SCREEN_CHAR_HEIGHT; 
             buffer_index++) [[likely]] {
            screen_buffer_ptr[buffer_index] = entry;
        }
    }


    /** 
     * @brief Makes a specific symbol color layout for the VGA-buffer.
     * 
     * @param foreground symbol color foreground
     * @param background symbol color background
     * @param blink      symbol color blink mode (default case `false`)
     * 
     * @return symbol color layout
     */
    _API_ 
    [[nodiscard]] 
    uint8_t
    Text_Mode::make_symbol_color(_IN_ const Text_Mode_Color foreground,
                                 _IN_ const Text_Mode_Color background,
                                 _IN_ const bool blink) noexcept {
        uint8_t color = (static_cast<uint8_t>(background) << 4) | 
                         static_cast<uint8_t>(foreground);

        if (blink) [[unlikely]] {
            color |= BLINK_MODE_BIT;
        }

        return color;
    }


    /**
     * @brief Outputs a character with a special fore and background color.
     * 
     * @note The color must match a specific layout, 
     *       use `Text_Mode::make_symbol_color` to ensure the 
     *       color layout is correct.
     * 
     * @param x      X coordinate for the output
     * @param y      Y coordinate for the output
     * @param color  character color
     * @param symbol character output
     * 
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_A`
     *          If `x` is greater then or equal to the screen width.
     * 
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
     *          If `y` is greater then or equal to the screen height.
     * 
     * @retval `status::SUCCESS`
     *          Default Case.
     */
    _API_ 
    status_t 
    Text_Mode::set_symbol_at(_IN_ const uint32_t x,
                             _IN_ const uint32_t y,
                             _IN_ const uint8_t color,
                             _IN_ const char symbol) noexcept {
        status_t status;
        uint32_t buffer_index;

        if (x >= SCREEN_CHAR_WIDTH) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_A;
            goto cleanup;
        }

        if (y >= SCREEN_CHAR_HEIGHT) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_B;
            goto cleanup;
        }

        buffer_index = y * SCREEN_CHAR_WIDTH + x;

        screen_buffer_ptr[buffer_index] = make_symbol_entry(symbol, color);

        status = status::SUCCESS;

    cleanup:
        return status;
    }


    /**
     * @brief Get the char and his color that was output at the 
     *        entered coordinate.
     * 
     * @param symbol outputted char
     * @param color  color of the outputted char
     * @param x      x coordinate of the outputted symbol
     * @param y      y coordinate of the outputted symbol
     * 
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_C`
     *          If `x` is greater or equal to the screen width.
     * 
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_D`
     *          If `y` is greater or equal to the screen height.
     *          
     * @retval `status::SUCCESS`
     */
    _API_
    status_t 
    Text_Mode::get_symbol_at(_OUT_ char& symbol,
                             _OUT_ uint8_t& color,
                             _IN_  const uint32_t x,
                             _IN_  const uint32_t y) noexcept {
        status_t status;
        uint16_t buffer_symbol_entry;

        if (x >= SCREEN_CHAR_WIDTH) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_C;
            goto cleanup;
        }

        if (y >= SCREEN_CHAR_HEIGHT) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_D;
            goto cleanup;
        }

        buffer_symbol_entry = screen_buffer_ptr[y * SCREEN_CHAR_WIDTH + x];

        symbol = static_cast<char>(buffer_symbol_entry & BYTE_MASK);
        color  = static_cast<uint8_t>((buffer_symbol_entry >> 8) & BYTE_MASK);

        status = status::SUCCESS;

    cleanup:
        return status;
    }


    /**
     * @brief Gets the screen width.
     * 
     * @note It's `static` and 
     *       can be called without any driver initialisation.
     * 
     * @return width of the screen
     */
    _API_
    uint32_t
    Text_Mode::screen_width() noexcept {
        return SCREEN_CHAR_WIDTH;
    }


    /**
     * @brief Gets the screen height.
     * 
     * @note It's `static` and 
     *       can be called without any driver initialisation.
     * 
     * @return height of the screen
     */
    _API_
    uint32_t
    Text_Mode::screen_height() noexcept {
        return SCREEN_CHAR_HEIGHT;
    }
} // namespace drivers::vga
