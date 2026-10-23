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

#include "interface.hpp"


namespace drivers::vga 
{
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


    class Text_Mode : public Interface<Text_Mode, Text_Mode_Color> {
    private:
        static constexpr uint32_t SCREEN_WIDTH  = 80;
        static constexpr uint32_t SCREEN_HEIGHT = 25;

        static constexpr uint8_t BLINK_MODE_BIT = 0x80;

        static constexpr uint16_t BYTE_MASK      = 0x00FF;
        static constexpr uint32_t COLOR_POSITION = 8;
        
        volatile uint16_t* const screen_buffer_ptr;


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
        make_symbol_entry(_IN_ const char symbol, 
                          _IN_ const uint8_t color) noexcept;


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
         * @brief Clears the screen and sets a special screen color.
         * 
         * @param foreground screen text color (default case `BLACK`)
         * @param background screen color (default case `BLACK`)
         */
        _API_ 
        void 
        clear_screen(_IN_ const Text_Mode_Color foreground,
                     _IN_ const Text_Mode_Color background) noexcept override;


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
        make_symbol_color(_IN_ const Text_Mode_Color foreground,
                          _IN_ const Text_Mode_Color background,
                          _IN_ const bool blink=false) noexcept override;


        /**
         * @brief Outputs a character with a special fore and background color.
         * 
         * @note The color must match a specific layout, 
         *       use `Text_Mode::make_symbol_color` 
         *       to ensure the color layout is correct.
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
        [[nodiscard]]
        status_t 
        set_symbol_at(_IN_ const uint32_t x,
                      _IN_ const uint32_t y,
                      _IN_ const uint8_t color,
                      _IN_ const char symbol) noexcept override;


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
        [[nodiscard]]
        status_t
        get_symbol_at(_OUT_ char& symbol,
                      _OUT_ uint8_t& color,
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
        constexpr 
        uint32_t 
        screen_height() noexcept {
            return SCREEN_HEIGHT;
        }
    };
} // namespace drivers::vga
