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


#include "text_mode.hpp"


namespace drivers::vga 
{
    uint16_t
    Text_Mode::assemble_buffer_entry(_IN_ const Text_Mode_Char& symbol) noexcept {
        uint16_t buffer_entry;

        buffer_entry = (static_cast<uint16_t>(symbol.background_color) & 0x0007) << 12 | 
                       (static_cast<uint16_t>(symbol.foreground_color) & 0x000F) << 8;

        if (symbol.blink_mode) [[unlikely]] {
            buffer_entry |= 0x8000;
        }

        buffer_entry |= static_cast<uint16_t>(static_cast<uint8_t>(symbol.symbol));

        return buffer_entry;
    }


    Text_Mode_Char
    Text_Mode::disassemble_buffer_entry(_IN_ const uint16_t symbol) noexcept {
        return {
            .background_color = static_cast<Text_Mode_Color>((symbol >> 12) & 0x0007),
            .foreground_color = static_cast<Text_Mode_Color>((symbol >> 8) & 0x000F),
            .blink_mode       = static_cast<bool>(symbol & 0x8000),
            .symbol           = static_cast<char>(symbol & 0x00FF)
        };
    }


    /**
     * @brief Initializes the VGA-textmode driver.
     */
    _API_
    Text_Mode::Text_Mode() noexcept :
        screen_buffer_ptr(reinterpret_cast<volatile uint16_t*>(0xB8000)) {
    }


    /** 
     * @brief Clears the screen and sets a special screen color.
     * 
     * @param screen_color screen color
     */
    _API_ 
    void 
    Text_Mode::clear_screen(_IN_ const Text_Mode_Char& screen_color) noexcept {
        const uint16_t buffer_entry = assemble_buffer_entry(screen_color);

        for (uint32_t buffer_index = 0; 
             buffer_index < SCREEN_WIDTH * SCREEN_HEIGHT; 
             buffer_index++) [[likely]] {
            screen_buffer_ptr[buffer_index] = buffer_entry;
        }
    }


    /**
     * @brief Outputs a character with a special fore and background color.
     * 
     * @param x      X coordinate for the output
     * @param y      Y coordinate for the output
     * @param color  character color
     * @param symbol character output with the colors
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
                             _IN_ const Text_Mode_Char& symbol) noexcept {
        status_t status;

        if (x >= SCREEN_WIDTH) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_A;
            goto cleanup;
        }

        if (y >= SCREEN_HEIGHT) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_B;
            goto cleanup;
        }

        screen_buffer_ptr[y * SCREEN_WIDTH + x] = assemble_buffer_entry(symbol);

        status = status::SUCCESS;

    cleanup:
        return status;
    }


    /**
     * @brief Get the char and his color that was output at the 
     *        entered coordinate.
     * 
     * @param symbol outputted char with the colors
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
    Text_Mode::get_symbol_at(_OUT_ Text_Mode_Char& symbol,
                             _IN_  const uint32_t x,
                             _IN_  const uint32_t y) noexcept {
        status_t status;
        uint16_t buffer_entry;

        if (x >= SCREEN_WIDTH) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_C;
            goto cleanup;
        }

        if (y >= SCREEN_HEIGHT) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_D;
            goto cleanup;
        }

        buffer_entry = screen_buffer_ptr[y * SCREEN_WIDTH + x];
        symbol       = disassemble_buffer_entry(buffer_entry);

        status = status::SUCCESS;

    cleanup:
        return status;
    }
} // namespace drivers::vga
