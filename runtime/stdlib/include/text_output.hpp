/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file contains the high-level text output system, 
    which provides formatted and unformatted print functions 
    on the 'VGA text mode driver'.

    It provides character output, string output, 
    integer formatting, hexadecimal and binary print output, 
    pointer formatting and supports setting various text and cursor colors.

    The 'Text_Output' class manages its own cursor state, 
    color configuration, and line management, 
    allowing structured and readable output during runtime.
    
NOTES:
    Some methods are placed in the header 
    because they are so small that the compiler can inline them.
*/


#pragma once

#include <types.hpp>
#include <drivers.hpp>
#include <sal.hpp>
#include <array.hpp>


namespace stdlib 
{
    class Text_Output final {
    private:
        static inline uint32_t cursor_x = 0;
        static inline uint32_t cursor_y = 0;
        
        static inline const bool does_blink = false;
        static inline uint8_t cursor_color  = 
            drivers::vga::Text_Mode::make_color(drivers::vga::Text_Mode_Colors::LIGHT_GREY,
                                                drivers::vga::Text_Mode_Colors::BLACK,
                                                does_blink);


        /** 
         * @brief Casts a integer with 
         *        a specific base to a string (eg. base 10 -> Decimal).
         * 
         * @note The resulting digits are stored in reverse order.
         * 
         * @warning No bounds checking is performed. 
         *          If the buffer is too small, the output will be truncated.
         * 
         * @param buffer casted integer
         * @param value  integer for casting
         * @param base   integer base
         * 
         * @return size of `buffer`
         */
        static uint32_t 
        convert_base_to_digits(_OUT_ stdlib::Array<char, 32>& buffer,
                               _IN_  uint32_t value,
                               _IN_  const uint32_t base) noexcept;


        /** 
         * @brief Outputs a integer to the terminal with 
         *        a specific number base (eg. base 10 -> Decimal). 
         * 
         * @warning Bases outside the range [2...16] are ignored.
         * 
         * @param value integer output
         * @param base  integer base
         */
        static void 
        put_base(_IN_ uint32_t value, 
                 _IN_ const uint32_t base) noexcept;
            

        /** 
         * @brief Casts a unsigned integer to a string.
         * 
         * @note The resulting digits are stored in reverse order.
         * 
         * @warning No bounds checking is performed. 
         *          If the buffer is too small, the output will be truncated.
         * 
         * @param buffer casted integer
         * @param value  integer for casting
         * 
         * @return size of `buffer`
         */
        static uint32_t 
        convert_uint_to_digits(_OUT_ stdlib::Array<char, 12>& buffer,
                               _IN_  uint32_t value) noexcept;


        /**
         * @brief Outputs `DELETE` at a specific position and 
         *        a new line if the screen width is reached.
         * 
         * @param symbol character output
         */
        static void 
        handle_backspace() noexcept;
        
        
        /**
         * @brief Outputs `TAB` at a specific position and 
         *        a new line if the screen width is reached.
         * 
         * @param symbol character output
         */
        static void 
        handle_tab() noexcept;
        

        /**
         * @brief Outputs normal characters at a specific position and 
         *        a new line if the screen width is reached.
         * 
         * @param symbol character output
         */
        static void 
        handle_normal_char(_IN_ const char symbol) noexcept;


        /** 
         * @brief Calculates all needed lines for a text output.
         * 
         * @note This function does not modify the cursor position.
         * 
         * @param text text for the line calculation
         * 
         * @return the count of needed lines.
         */
        static uint32_t 
        calculate_needed_lines(_IN_ const char* text) noexcept;


        /** 
         * Makes a cursor Jump to the next line.
         */
        static void 
        new_line() noexcept;


    public:
        /** 
         * @brief Gets the X coordinate of the cursor
         * 
         * @return cursor X coordinate
         */        
        _API_ [[nodiscard]] static inline uint32_t 
        get_cursor_x() noexcept {
            return cursor_x;
        }


        /** 
         * @brief Gets the Y coordinate of the cursor
         * 
         * @return cursor Y coordinate
         */   
        _API_ [[nodiscard]] static inline uint32_t 
        get_cursor_y() noexcept {
            return cursor_y;
        }


        /**
         * @brief Gets the current text color.
         * 
         * @return text color
         */
        _API_ [[nodiscard]] static inline uint8_t 
        get_text_color() noexcept {
            return cursor_color;
        }


        /** 
         * @brief Sets a new color for any other Terminal outputs.
         * 
         * @param color      color for symbols
         * @param background background color for symbols
         */
        _API_ static inline void 
        set_text_color(_IN_ const drivers::vga::Text_Mode_Colors& color,
                       _IN_ const drivers::vga::Text_Mode_Colors& background,
                       _IN_ const bool does_blink=false) noexcept {
            cursor_color = drivers::vga::Text_Mode::make_color(color, 
                                                               background, 
                                                               does_blink);
        }


        /** 
         * @brief Sets a new cursor position.
         * 
         * @param x X coordinate
         * @param y Y coordinate
         */
        _API_ static inline constexpr void
        set_cursor(_IN_ const uint32_t x, 
                   _IN_ const uint32_t y) noexcept {
            cursor_x = x;
            cursor_y = y;
        }


        /**
         * @brief Clears the screen and resets the cursor.
         */
        _API_ static inline void 
        reset() noexcept {
            using namespace drivers;

            cursor_x = 0;
            cursor_y = 0;

            vga::Text_Mode::clear_screen(vga::Text_Mode_Colors::BLACK);
        }


        /** 
         * @brief Puts a signed integer.
         * 
         * @param value integer output
         */
        _API_ static void 
        put_int(_IN_  int32_t value) noexcept;


        /** 
         * @brief Outputs a unsigned integer to the terminal.
         * 
         * @param value integer output
         */
        _API_ static void 
        put_uint(_IN_ const uint32_t value) noexcept;


        /** 
         * @brief Outputs a hexadecimal number to the terminal.
         * 
         * @param value number output
         */
        _API_ static inline void
        put_hex(_IN_ const uint32_t value) noexcept {
            put_string("0x");
            put_base(value, 16);
        }


        /** 
         * @brief Outputs a binary number to the terminal.
         * 
         * @param value number output
         */
        _API_ static inline void 
        put_bin(_IN_ const uint32_t value) noexcept {
            put_string("0b");
            put_base(value, 2);
        }


        /** 
         * @brief Outputs the address of pointer to the 
         *        terminal in hexadecimal.
         * 
         * @param value address output
         */
        _API_ static inline void 
        put_ptr(_IN_ const uint32_t value) noexcept {
            put_string("0x");
            put_base(value, 16);
        }


        /** 
         * @brief Outputs a character to the terminal.
         * 
         * @param symbol character output
         * 
         * @note Allows the special characters:
         * @note - `\r` -> Cursor jump to the line begin.
         * @note - `\b` -> Delete the last character.
         * @note - `\t` -> Outputs a TAB.
         * @note - `\n` -> Cursor jump to the next line.
         * @note - `\\"` -> Allows `"` as character output.
         */
        _API_ static void 
        put_char(_IN_ const char symbol) noexcept;


        /** 
         * @brief Outputs a string to the terminal.
         * 
         * @param message string output
         * 
         * @note Allows the special characters:
         * @note - `\r` -> Cursor jump to the line begin.
         * @note - `\b` -> Delete the last character.
         * @note - `\t` -> Outputs a TAB.
         * @note - `\n` -> Cursor jump to the next line.
         * @note - `\\"` -> allows `"` in the string output
         */
        _API_ static void 
        put_string(_IN_ const char* message) noexcept;
    

        Text_Output() noexcept  = default;
        ~Text_Output() noexcept = default;
    };
} // namespace stdlib
