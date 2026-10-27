/**
 * @brief Implements an driver for VGA text-mode outputs 
 *        (16 color 25x80 console).
 *
 * @copyright Copyright (c) 2026 Lukenar11 (Luke Matthes)
 * @license:  https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
 * @author    Lukenar11 (Luke Matthes)
 *
 * @note
 */


#include "text_mode.hpp"


namespace drivers::vga 
{
    /**
     * @brief Converts the high-level character representation into the
     *        low-level 16-bit VGA text-mode format.
     * 
     * @param buffer_entry Output VGA buffer entry.
     * @param symbol       Text-mode character representation with
     *                     foreground color, background color,
     *                     blink mode and symbol.
     * 
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
     *          If the foreground or background color value is out of range.
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     */
    [[nodiscard]]
    status_t
    Text_Mode::assemble_buffer_entry(_OUT_ uint16_t& buffer_entry,
                                     _IN_  const Text_Mode_Char& symbol)
                                     noexcept {
        status_t status = status::SUCCESS;

        buffer_entry = 0;

        if (static_cast<uint16_t>(symbol.background_color) > 
            BACKGROUND_MASK) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_B;
            goto cleanup;
        }

        if (static_cast<uint16_t>(symbol.foreground_color) > 
            FOREGROUND_MASK) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_B;
            goto cleanup;
        }

        buffer_entry = (static_cast<uint16_t>(symbol.blink_mode) << 
                        BLINK_MODE_SHIFT) | 
                       (static_cast<uint16_t>(symbol.background_color) << 
                        BACKGROUND_SHIFT) |
                       (static_cast<uint16_t>(symbol.foreground_color) << 
                        FOREGROUND_SHIFT) | 
                        static_cast<uint8_t>(symbol.symbol);
 
    cleanup:
        return status;
    }


    /**
     * @brief Converts the low-level 16-bit VGA text-mode format into a 
     *        high-level character representation.
     *
     * @param symbol VGA buffer entry.
     *
     * @return Text-mode character representation with foreground color,
     *         background color, blink mode and symbol.
     */
    [[nodiscard]]
    const
    Text_Mode_Char
    Text_Mode::disassemble_buffer_entry(_IN_ const uint16_t symbol) noexcept {
        return {
            .background_color = static_cast<Text_Mode_Color>((symbol >> 
                                                              BACKGROUND_SHIFT) & 
                                                              BACKGROUND_MASK),
            .foreground_color = static_cast<Text_Mode_Color>((symbol >> 
                                                              FOREGROUND_SHIFT) & 
                                                              FOREGROUND_MASK),
            .blink_mode = static_cast<bool>(symbol & BLINK_MODE_MASK),
            .symbol     = static_cast<char>(symbol & SYMBOL_MASK)
        };
    }


    /**
     * @brief Initializes the VGA-textmode driver.
     */
    _API_
    Text_Mode::Text_Mode() noexcept :
        screen_buffer_ptr(reinterpret_cast<volatile uint16_t*>(0xB8000)),
        screen_buffer_size(SCREEN_HEIGHT * SCREEN_WIDTH) {
    }


    /**
     * @brief Fills the screen with the specified symbol.
     *
     * @param filler_symbol Symbol used to fill the screen.
     * 
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_A`
     *          If the foreground or background color value is out of range.
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_ 
    [[nodiscard]]
    status_t 
    Text_Mode::fill_screen(_IN_ const Text_Mode_Char& filler_symbol) noexcept {
        uint16_t buffer_entry = 0;
        status_t status       = assemble_buffer_entry(buffer_entry, 
                                                      filler_symbol);

        if (status != status::SUCCESS) [[unlikely]] {
            // The `PARAM_B` flag is changed to `PARAM_A`, 
            // so that the returned status code matches the method signature.
            status ^= status::flags::PARAM_B;
            status |= status::flags::PARAM_A;

            goto cleanup;
        }

        for (uint32_t entry_number = 0; 
             entry_number < screen_buffer_size; 
             entry_number++) [[likely]] {
            screen_buffer_ptr[entry_number] = buffer_entry;
        }

    cleanup:
        return status;
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
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_C`
     *          If the foreground or background color value is out of range.
     * 
     * @retval `status::SUCCESS`
     *          Default Case.
     */
    _API_ 
    [[nodiscard]]
    status_t 
    Text_Mode::set_symbol_at(_IN_ const uint32_t x,
                             _IN_ const uint32_t y,
                             _IN_ const Text_Mode_Char& symbol) noexcept {
        status_t status       = status::SUCCESS;
        uint16_t buffer_entry = 0;

        if (x >= SCREEN_WIDTH) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_A;
            goto cleanup;
        }

        if (y >= SCREEN_HEIGHT) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_B;
            goto cleanup;
        }

        status = assemble_buffer_entry(buffer_entry, symbol);
        if (status & status::flags::PARAM_B) [[unlikely]] {
            // The `PARAM_B` flag is changed to `PARAM_C`, 
            // so that the returned status code matches the method signature.
            status ^= status::flags::PARAM_B;
            status |= status::flags::PARAM_C;

            goto cleanup;
        }

        screen_buffer_ptr[y * SCREEN_WIDTH + x] = buffer_entry;

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
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
     *          If `x` is greater or equal to the screen width.
     * 
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_C`
     *          If `y` is greater or equal to the screen height.
     *          
     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_
    [[nodiscard]]
    status_t 
    Text_Mode::get_symbol_at(_OUT_ Text_Mode_Char& symbol,
                             _IN_  const uint32_t x,
                             _IN_  const uint32_t y) noexcept {
        status_t status       = status::SUCCESS;
        uint16_t buffer_entry = 0;

        symbol = {};

        if (x >= SCREEN_WIDTH) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_B;
            goto cleanup;
        }

        if (y >= SCREEN_HEIGHT) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_C;
            goto cleanup;
        }

        buffer_entry = screen_buffer_ptr[y * SCREEN_WIDTH + x];
        symbol       = disassemble_buffer_entry(buffer_entry);

    cleanup:
        return status;
    }
} // namespace drivers::vga
