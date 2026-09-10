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


#include <text_output.hpp>


namespace stdlib
{
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
    uint32_t 
    Text_Output::convert_base_to_digits(_OUT_ stdlib::Array<char, 32>& buffer,
                                        _IN_  uint32_t value,
                                        _IN_  const uint32_t base) noexcept {
        uint32_t chars_written   = 0;
        uint8_t converted_number = 0;
        const char null_char     = '0';
        const char a_char        = 'A';
        const uint32_t ten       = 10;

        if (base < 2 || base > 16) [[unlikely]] {
            goto cleanup;
        }

        if (value == 0) [[unlikely]] {
            put_char('0');
            goto cleanup;
        }

        while (value) [[likely]] {
            if (chars_written >= buffer.size()) [[unlikely]] {
                break;
            }

            converted_number = value % base;
            if (converted_number < ten) {
                buffer[chars_written++] = null_char + converted_number;
            }
            else {
                buffer[chars_written++] = a_char + converted_number - ten;
            }

            value /= base;
        }

    cleanup:
        return chars_written;
    }


    /** 
     * @brief Outputs a integer to the terminal with 
     *        a specific number base (eg. base 10 -> Decimal). 
     * 
     * @warning Bases outside the range [2...16] are ignored.
     * 
     * @param value integer output
     * @param base  integer base
     */
    void 
    Text_Output::put_base(_IN_ uint32_t value, 
                          _IN_ const uint32_t base) noexcept {
        stdlib::Array<char, 32> buffer;
        uint32_t buffer_count;

        if (base < 2 || base > 16) [[unlikely]] {
            goto cleanup;
        }

        if (value == 0) [[unlikely]] {
            put_char('0');
            goto cleanup;
        }

        buffer_count = convert_base_to_digits(buffer, value, base);

        while (buffer_count--) [[likely]] {
            put_char(buffer[buffer_count]);
        }

    cleanup:
        return;
    }


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
    uint32_t 
    Text_Output::convert_uint_to_digits(_OUT_ stdlib::Array<char, 12>& buffer,
                                        _IN_  uint32_t value) noexcept {
        uint32_t buffer_count = 0;
        const char null_char  = '0';
        const uint32_t ten    = 10;

        while (value) [[likely]] {
            if (buffer_count >= buffer.size()) [[unlikely]] {
                break;
            }

            buffer[buffer_count++] = null_char + (value % ten);
            value /= ten;
        }

        return buffer_count;
    }


    /**
     * @brief Outputs `DELETE` at a specific position and 
     *        a new line if the screen width is reached.
     * 
     * @param symbol character output
     */
    void 
    Text_Output::handle_backspace() noexcept {
        if (cursor_x > 0) [[likely]] {
            cursor_x--;
            drivers::vga::Text_Mode::put_char_at(cursor_x,
                                                 cursor_y,
                                                 cursor_color,
                                                 ' ');
        }
    }


    /**
     * @brief Outputs `TAB` at a specific position and 
     *        a new line if the screen width is reached.
     * 
     * @param symbol character output
     */
    void 
    Text_Output::handle_tab() noexcept {
        for (uint32_t space_count = 0; 
             space_count < 4; 
             space_count++) [[likely]] {
            drivers::vga::Text_Mode::put_char_at(cursor_x,
                                                 cursor_y,
                                                 cursor_color,
                                                 ' ');
            cursor_x++;

            if (cursor_x >= drivers::vga::TEXT_MODE_SCREEN_WIDTH) [[unlikely]] {
                new_line();
            }
        }
    }


    /**
     * @brief Outputs normal characters at a specific position and 
     *        a new line if the screen width is reached.
     * 
     * @param symbol character output
     */
    void 
    Text_Output::handle_normal_char(_IN_ const char symbol) noexcept {
        drivers::vga::Text_Mode::put_char_at(cursor_x,
                                             cursor_y,
                                             cursor_color,
                                             symbol);
        cursor_x++;

        if (cursor_x >= drivers::vga::TEXT_MODE_SCREEN_WIDTH) [[unlikely]] {
            new_line();
        }
    }


    /** 
     * @brief Calculates all needed lines for a text output.
     * 
     * @note This function does not modify the cursor position.
     * 
     * @param text text for the line calculation
     * 
     * @return the count of needed lines.
     */
    uint32_t 
    Text_Output::calculate_needed_lines(_IN_ const char* text) noexcept {
        const uint32_t null   = 0;
        uint32_t needed_lines = 1;
        uint32_t x            = cursor_x;
        char symbol;

        if (!text || text[0] == '\0') [[unlikely]] {
            needed_lines = 0;
            goto cleanup;
        }

        while (*text) [[likely]] {
            symbol = *text++;
            if (symbol == '\n') [[unlikely]] {
                needed_lines++;
                x = null;
                continue;
            }

            x++;
            if (x >= drivers::vga::TEXT_MODE_SCREEN_WIDTH) [[unlikely]] {
                needed_lines++;
                x = null;
            }
        }

    cleanup:
        return needed_lines;
    }


    /** 
     * Makes a cursor Jump to the next line.
     */
    void 
    Text_Output::new_line() noexcept {
        cursor_x = 0;
        cursor_y++;

        if (cursor_y >= drivers::vga::TEXT_MODE_SCREEN_HEIGHT) [[unlikely]] {
            reset();
        }
    }


    /** 
     * @brief Puts a signed integer.
     * 
     * @param value integer output
     */
    _API_ 
    void 
    Text_Output::put_int(_IN_ int32_t value) noexcept {
        if (value < 0) {
            put_char('-');

            value = static_cast<uint32_t>(-(value + 1)) + 1;
            put_uint(value);
        }
        else {
            put_uint(value);
        }
    }


    /** 
     * @brief Outputs a unsigned integer to the terminal.
     * 
     * @param value integer output
     */
    _API_ 
    void 
    Text_Output::put_uint(_IN_ const uint32_t value) noexcept {
        uint32_t char_count;
        stdlib::Array<char, 12> buffer;
        
        if (value == 0) [[unlikely]] {
            put_char('0');
            goto cleanup;
        }

        char_count = convert_uint_to_digits(buffer, value);

        while (char_count--) [[likely]] {
            put_char(buffer[char_count]);
        }

    cleanup:
        return;
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
    _API_ 
    void 
    Text_Output::put_char(_IN_ const char symbol) noexcept {
        switch (symbol) {
        case '\r':
            cursor_x = 0;
            break;
        
        case '\b':
            handle_backspace();
            break;
        
        case '\t':
            handle_tab();
            break;
        
        case '\n':
            new_line();
            break;

        case '\0':
            break;
        
        default:
            handle_normal_char(symbol);
            break;
        }
    }


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
    _API_ 
    void
    Text_Output::put_string(_IN_ const char* message) noexcept {
        uint32_t needed_lines;

        if (!message || message[0] == '\0') [[unlikely]] {
            goto cleanup;
        }

        needed_lines = calculate_needed_lines(message);
        
        if (needed_lines > 
            drivers::vga::TEXT_MODE_SCREEN_HEIGHT - cursor_y) [[unlikely]] {
            reset();
        }
        while (*message) [[likely]] {
            put_char(*message++);
        }

    cleanup:
        return;
    }
} // namespace stdlib
