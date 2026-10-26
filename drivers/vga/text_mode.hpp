/**
 * @brief Defines an driver for VGA text-mode outputs.
 *
 * @copyright Copyright (c) 2026 Lukenar11 (Luke Matthes)
 * @details   https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
 * @author    Lukenar11 (Luke Matthes)
 *
 * @note
 */


#pragma once

#include "interface.hpp"


namespace drivers::vga
{
    /**
     * @brief Defines the available VGA text-mode colors.
     * 
     * @details The values ​​correspond to the 4-bit VGA text mode color index 
     *          for the foreground and background colors.
     */
    enum class Text_Mode_Color : uint8_t {
        BLACK         = 0x00,
        BLUE          = 0x01,
        GREEN         = 0x02,
        CYAN          = 0x03,
        RED           = 0x04,
        MAGENTA       = 0x05,
        BROWN         = 0x06,
        LIGHT_GREY    = 0x07,
        DARK_GREY     = 0x08,
        LIGHT_BLU     = 0x09,
        LIGHT_GREEN   = 0x0A,
        LIGHT_CYAN    = 0x0B,
        LIGHT_RED     = 0x0C,
        LIGHT_MAGENTA = 0x0D,
        YELLOW        = 0x0E,
        WHITE         = 0x0F
    };


    /**
     * @brief Represents a single VGA text-mode character cell.
     * 
     * @note `background_color` is set to 
     *       `Text_Mode_Color::BLACK` by default.
     * 
     * @note `foreground_color` is set to 
     *       `Text_Mode_Color::LIGHT_GREY` by default.
     * 
     * @note `blink_mode` is set to 
     *       `false` by default.
     * 
     * @note `symbol` is set to 
     *       `' '` by default.
     */
    struct Text_Mode_Char final {
        Text_Mode_Color background_color = Text_Mode_Color::BLACK;
        Text_Mode_Color foreground_color = Text_Mode_Color::LIGHT_GREY;
        bool blink_mode                  = false;
        char symbol                      = ' ';
    };


    /**
     * @brief Provides a VGA text-mode driver for screen output.
     * 
     * @details Implements character-based access to the 
     *          standard 80x25 VGA text buffer.
     * 
     * @details Supports writing individual characters, filling the screen, 
     *          reading cells from the screen content, and 
     *          converting between an abstract character representation and the
     *          low-level 16-bit VGA text mode format.
     */
    class Text_Mode : public Interface<Text_Mode, Text_Mode_Char> {
    private:
        static constexpr uint32_t SCREEN_WIDTH  = 80;
        static constexpr uint32_t SCREEN_HEIGHT = 25;

        static constexpr uint32_t BLINK_MODE_SHIFT = 15;
        static constexpr uint32_t BACKGROUND_SHIFT = 12;
        static constexpr uint32_t FOREGROUND_SHIFT = 8;

        static constexpr uint16_t BLINK_MODE_MASK = 0x8000;
        static constexpr uint16_t BACKGROUND_MASK = 0x0007;
        static constexpr uint16_t FOREGROUND_MASK = 0x000F;
        static constexpr uint16_t SYMBOL_MASK     = 0x00FF;

        volatile uint16_t* const screen_buffer_ptr;
        const uint32_t screen_buffer_size;


        /**
         * @brief Converts the high-level character representation into the
         *        low-level 16-bit VGA text-mode format.
         *
         * @param buffer_entry Output VGA buffer entry.
         * @param symbol       Text-mode character representation with
         *                     foreground color, background color,
         *                     blink mode and symbol.
         * 
         * @retval `status::INVALID_PARAMETER | status::flags::BACKGROUND_COLOR`
         *          If the background color value is out of range.
         * 
         * @retval `status::INVALID_PARAMETER | status::flags::FOREGROUND_COLOR`
         *          If the foreground color value is out of range.
         * 
         * @retval `status::SUCCESS`
         *          Default case.
         */
        [[nodiscard]]
        status_t
        assemble_buffer_entry(_OUT_ uint16_t& buffer_entry,
                              _IN_  const Text_Mode_Char& symbol) noexcept;


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
        disassemble_buffer_entry(_IN_ const uint16_t symbol) noexcept;


    public:
        /**
         * @brief Initializes the VGA-textmode driver.
         */
        _API_
        Text_Mode() noexcept;


        /**
         * @brief Uninitializes the VGA-textmode driver
         * 
         * @note It's declared as default.
         */
        _API_
        ~Text_Mode() noexcept = default;


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
        fill_screen(_IN_ const Text_Mode_Char& filler_symbol)
                    noexcept override;


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
        set_symbol_at(_IN_ const uint32_t x,
                      _IN_ const uint32_t y,
                      _IN_ const Text_Mode_Char& symbol) noexcept override;


        /**
         * @brief Get the char and his color that was output at the 
         *        entered coordinate.
         * 
         * @param symbol outputted char with the color
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
        get_symbol_at(_OUT_ Text_Mode_Char& symbol,
                      _IN_  const uint32_t x,
                      _IN_  const uint32_t y) noexcept override;


        /**
         * @brief Gets the screen width.
         * 
         * @note It's `static` and 
         *       can be called without any driver initialisation.
         * 
         * @return width of the screen
         */
        _API_
        [[nodiscard]]
        static
        inline
        constexpr
        uint32_t
        screen_width() noexcept {
            return SCREEN_WIDTH;
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
        [[nodiscard]]
        static
        inline
        constexpr
        uint32_t
        screen_height() noexcept {
            return SCREEN_HEIGHT;
        }
    };
} // namespace drivers::vga
