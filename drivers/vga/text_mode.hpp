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


    struct Text_Mode_Char final {
        Text_Mode_Color background_color = Text_Mode_Color::BLACK;
        Text_Mode_Color foreground_color = Text_Mode_Color::LIGHT_GREY;
        bool blink_mode                  = false;
        char symbol                      = ' ';
    };


    class Text_Mode : public Interface<Text_Mode, Text_Mode_Char> {
    private:
        static constexpr uint32_t SCREEN_WIDTH  = 80;
        static constexpr uint32_t SCREEN_HEIGHT = 25;
        
        volatile uint16_t* const screen_buffer_ptr;


        uint16_t
        assemble_buffer_entry(_IN_ const Text_Mode_Char& symbol) noexcept;


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
         * @brief Clears the screen and sets a special screen color.
         *
         * @param screen_color screen color
         */
        _API_ 
        void 
        clear_screen(_IN_ const Text_Mode_Char& screen_color) noexcept override;


        /**
         * @brief Outputs a character with a special fore and background color.
         * 
         * @param x      X coordinate for the output
         * @param y      Y coordinate for the output
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
