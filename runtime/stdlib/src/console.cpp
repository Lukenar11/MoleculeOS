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


#include <console.hpp>
#include <drivers.hpp>


namespace
{
    using namespace stdlib;
    using namespace console;


    uint32_t cursor_x = 0;
    uint32_t cursor_y = 0;
        
    uint8_t 
    cursor_color = drivers::vga::Text_Mode::make_color(drivers::vga::Text_Mode_Colors::LIGHT_GREY,
                                                       drivers::vga::Text_Mode_Colors::BLACK,
                                                       false);


    /** 
     * Makes a cursor Jump to the next line.
     */
    void 
    new_line() noexcept {
        cursor_x = 0;
        cursor_y++;

        if (cursor_y >= drivers::vga::TEXT_MODE_SCREEN_HEIGHT) [[unlikely]] {
            reset();
        }
    }

    
    /** 
     * @brief Casts a integer with 
     *        a specific base to a string (eg. base 10 -> Decimal).
     * 
     * @note The resulting digits are stored in reverse order.
     * 
     * @warning No bounds checking is performed. 
     *          If the buffer is too small, the output will be truncated.
     * 
     * @param buffer        casted integer
     * @param chars_written size of `buffer`
     * @param value         integer for casting
     * @param base          integer base
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *          If `buffer` is a `nullptr`
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_C`
     *          If `value` is a `nullptr`
     * 
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_D`
     *          If `base is not` between 2 and 16.
     * 
     * @retval `status::FAIL` 
     *          Unknown error.
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     */
    [[nodiscard]]
    status_t 
    convert_base_to_digits(_OUT_ stdlib::Array<char, 32>& buffer,
                           _OUT_ uint32_t& chars_written,
                           _IN_  uint32_t value,
                           _IN_  const uint32_t base) noexcept {
        status_t status;
        uint8_t converted_number = 0;
        const char null_char     = '0';
        const char a             = 'A';
        const uint32_t ten       = 10;

        chars_written = 0;

        if (!buffer.data()) [[unlikely]] {
            status = status::NULL_POINTER | status::flags::PARAM_A;
            goto cleanup;
        }

        if (!value) [[unlikely]] {
            status = status::NULL_POINTER | status::flags::PARAM_C;
            goto cleanup;
        }

        if (base < 2 || base > 16) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_D;
            goto cleanup;
        }

        if (value == 0) [[unlikely]] {
            status = put_char('0');
            goto cleanup;
        }

        while (value) [[likely]] {
            if (chars_written >= buffer.size()) {
                break;
            }

            converted_number = value % base;
            if (converted_number < ten) {
                buffer[chars_written++] = null_char + converted_number;
            }
            else {
                buffer[chars_written++] = a + converted_number - ten;
            }

            value /= base;
        }

        status = status::SUCCESS;

    cleanup:
        return status;
    }


    /** 
     * @brief Outputs a integer to the terminal with 
     *        a specific number base (eg. base 10 -> Decimal). 
     * 
     * @warning Bases outside the range [2...16] are ignored.
     * 
     * @param value integer output
     * @param base  integer base
     * 
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
     *          If `base is not` between 2 and 16.
     * 
     * @retval `status::FAIL`
     *          Unknown Error.
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     */
    [[nodiscard]]
    status_t 
    put_base(_IN_ const uint32_t value, 
             _IN_ const uint32_t base) noexcept {
        stdlib::Array<char, 32> buffer;
        uint32_t buffer_count;
        status_t status;

        if (base < 2 || base > 16) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_B;
            goto cleanup;
        }

        if (value == 0) [[unlikely]] {
            put_char('0');
            status = status::SUCCESS;
            goto cleanup;
        }

        status = convert_base_to_digits(buffer, buffer_count, value, base);
        if (status != status::SUCCESS) [[unlikely]] {
            status = status::FAIL;
            goto cleanup;
        }

        while (buffer_count--) [[likely]] {
            status = put_char(buffer[buffer_count]);
            if (status != status::SUCCESS) [[unlikely]] {
                goto cleanup;
            }
        }

        status = status::SUCCESS;

    cleanup:
        return status;
    }


    /** 
     * @brief Casts a unsigned integer to a string.
     * 
     * @note The resulting digits are stored in reverse order.
     * 
     * @warning No bounds checking is performed. 
     *          If the buffer is too small, the output will be truncated.
     * 
     * @param buffer       casted integer
     * @param buffer_count size of `buffer`
     * @param value        integer for casting
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *          If the budder is a `nullptr`.
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_C`
     *          If `value` is a `nullptr`
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     */
    [[nodiscard]]
    status_t 
    convert_uint_to_digits(_OUT_ stdlib::Array<char, 12>& buffer,
                           _OUT_ uint32_t& buffer_count,
                           _IN_  uint32_t value) noexcept {
        status_t status;
        const char null_char  = '0';
        const uint32_t ten    = 10;

        if (!buffer.data()) [[unlikely]] {
            status = status::NULL_POINTER | status::flags::PARAM_A;
            goto cleanup;
        }

        if (!value) [[unlikely]] {
            status = status::NULL_POINTER | status::flags::PARAM_C;
            goto cleanup;
        }

        while (value) [[likely]] {
            if (buffer_count >= buffer.size()) {
                break;
            }

            buffer[buffer_count++] = null_char + (value % ten);
            value                 /= ten;
        }

        status = status::SUCCESS;

    cleanup:
        return status;
    }


    /**
     * @brief Outputs `DELETE` at a specific position and 
     *        a new line if the screen width is reached.
     * 
     * @param symbol character output
     * 
     * @retval `status::FAIL`
     *          If the char handling fails.
     * 
     * @retval `status::SUCCESS`
     *          Default Case.
     */
    [[nodiscard]]
    status_t 
    handle_backspace() noexcept {
        status_t status;

        if (cursor_x > 0) [[likely]] {
            cursor_x--;
            status = drivers::vga::Text_Mode::put_char_at(cursor_x,
                                                          cursor_y,
                                                          cursor_color,
                                                          ' ');
            if (status != status::SUCCESS) [[unlikely]] {
                status = status::FAIL;
            }
        }
        else {
            status = status::SUCCESS;
        }

        return status;
    }


    /**
     * @brief Outputs `TAB` at a specific position and 
     *        a new line if the screen width is reached.
     * 
     * @param symbol character output
     * 
     * @retval `status::FAIL`
     *          If the char handling fails.
     * 
     * @retval `status::SUCCESS`
     *          Default Case.
     */
    [[nodiscard]]
    status_t 
    handle_tab() noexcept {
        status_t status;

        for (uint32_t puted_spaces = 0; 
             puted_spaces < 4 - (cursor_y % 4);
             puted_spaces++) [[likely]] {
            status = drivers::vga::Text_Mode::put_char_at(cursor_x,
                                                          cursor_y,
                                                          cursor_color,
                                                          ' ');
            if (status != status::SUCCESS) [[unlikely]] {
                status = status::FAIL;
                break;
            }

            cursor_x++;

            if (cursor_x >= 
                drivers::vga::TEXT_MODE_SCREEN_WIDTH) [[unlikely]] {
                new_line();
            }
        }

        return status;
    }


    /**
     * @brief Outputs normal characters at a specific position and 
     *        a new line if the screen width is reached.
     * 
     * @param symbol character output
     * 
     * @retval `status::FAIL`
     *          If the char handling fails.
     * 
     * @retval `status::SUCCESS` 
     *          Default Case.
     */
    [[nodiscard]]
    status_t 
    handle_normal_char(_IN_ const char symbol) noexcept {
        status_t status = drivers::vga::Text_Mode::put_char_at(cursor_x,
                                                               cursor_y,
                                                               cursor_color,
                                                               symbol);
        if (status != status::SUCCESS) [[unlikely]] {
            status = status::FAIL;
            goto cleanup;
        }

        cursor_x++;

        if (cursor_x >= drivers::vga::TEXT_MODE_SCREEN_WIDTH) [[unlikely]] {
            new_line();
        }

    cleanup:
        return status;
    }


    /** 
     * @brief Calculates all needed lines for a text output.
     * 
     * @note This function does not modify the cursor position.
     * 
     * @param needed_lines count of needed lines
     * @param text         text for the line calculation
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_B`
     *          If `text` is a `nullptr`.
     * 
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
     *          If `text` is empty.
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     */
    [[nodiscard]]
    status_t 
    calculate_needed_lines(_OUT_ uint32_t& needed_lines, 
                           _IN_  const char* const text) noexcept {
        const char new_line = '\n';
        uint32_t x          = cursor_x;
        char symbol;
        status_t status;

        needed_lines = 1;

        if (!text) [[unlikely]] {
            needed_lines = 0;
            status       = status::NULL_POINTER | status::flags::PARAM_B;
            goto cleanup;
        }

        if (text[0] == '\0') [[unlikely]] {
            needed_lines = 0;
            status       = status::INVALID_PARAMETER | status::flags::PARAM_B;
            goto cleanup;
        }

        for (uint32_t text_char = 0; 
             text[text_char] != '\0';
             text_char++) [[likely]] {
            symbol = text[text_char];
            if (symbol == new_line) [[unlikely]] {
                needed_lines++;
                x ^= x;
                continue;
            }

            x++;
            if (x >= drivers::vga::TEXT_MODE_SCREEN_WIDTH) [[unlikely]] {
                needed_lines++;
                x ^= x;
            }
        }

        status = status::SUCCESS;

    cleanup:
        return status;
    }
} // namespace


namespace stdlib::console
{
    /** 
     * @brief Gets the X coordinate of the cursor
     * 
     * @return cursor X coordinate
     */        
    _API_ 
    [[nodiscard]] 
    uint32_t 
    get_cursor_x() noexcept {
        return cursor_x;
    }


    /** 
     * @brief Gets the Y coordinate of the cursor
     * 
     * @return cursor Y coordinate
     */   
    _API_ 
    [[nodiscard]] 
    uint32_t 
    get_cursor_y() noexcept {
        return cursor_y;
    }


    /**
     * @brief Gets the current text color.
     * 
     * @return text color
     */
    _API_ 
    [[nodiscard]] 
    uint8_t 
    get_text_color() noexcept {
        return cursor_color;
    }


    /** 
     * @brief Sets a new color for any other Terminal outputs.
     * 
     * @param color      color for symbols
     * @param background background color for symbols
     */
    _API_ 
    void 
    set_text_color(_IN_ const drivers::vga::Text_Mode_Colors color,
                   _IN_ const drivers::vga::Text_Mode_Colors background,
                   _IN_ const bool does_blink) noexcept {
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
    _API_ 
    void
    set_cursor(_IN_ const uint32_t x, 
               _IN_ const uint32_t y) noexcept {
        if (x < drivers::vga::TEXT_MODE_SCREEN_WIDTH &&
            y < drivers::vga::TEXT_MODE_SCREEN_HEIGHT) [[likely]] {
            cursor_x = x;
            cursor_y = y;
        }
    }


    /**
     * @brief Clears the screen and resets the cursor.
     */
    _API_  
    void 
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
     *  
     * @retval `status::FAIL`
     *          Unknown error.
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_ 
    status_t 
    put_int(_IN_ int32_t value) noexcept {
        status_t status;

        if (value < 0) {
            status = put_char('-');
            if (status != status::SUCCESS) [[unlikely]] {
                goto cleanup;
            }

            value  = static_cast<uint32_t>(-(value + 1)) + 1;
            status = put_uint(value);
        }
        else {
            status = put_uint(value);
        }

    cleanup:
        return status;
    }


    /** 
     * @brief Outputs a unsigned integer to the terminal.
     * 
     * @param value integer output
     * 
     * @retval `status::FAIL`
     *          Unknown error.
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_
    status_t 
    put_uint(_IN_ const uint32_t value) noexcept {
        uint32_t char_count;
        stdlib::Array<char, 12> buffer;
        status_t status;
        
        if (value == 0) [[unlikely]] {
            status = put_char('0');
            goto cleanup;
        }

        status = convert_uint_to_digits(buffer, char_count, value);
        if (status != status::SUCCESS) [[unlikely]] {
            status = status::FAIL;
            goto cleanup;
        }

        while (char_count--) [[likely]] {
            status = put_char(buffer[char_count]);

            if (status != status::SUCCESS) [[unlikely]] {
                break;
            }
        }

    cleanup:
        return status;
    }


    /** 
     * @brief Outputs a hexadecimal number to the terminal.
     * 
     * @retval `status::FAIL`
     *          Unknown error.
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     * 
     * @param value number output
     */
    _API_  
    status_t
    put_hex(_IN_ const uint32_t value) noexcept {
        status_t status;

        status = put_string("0x");
        if (status != status::SUCCESS) [[unlikely]] {
            status = status::FAIL;
        }
        else {
            status = put_base(value, 16);
            if (status & status::flags::PARAM_B) {
                status = status::FAIL;
            }
        }

        return status;
    }


    /** 
     * @brief Outputs a binary number to the terminal.
     * 
     * @retval `status::FAIL`
     *          Unknown error.
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     * 
     * @param value number output
     */
    _API_   
    status_t 
    put_bin(_IN_ const uint32_t value) noexcept {
        status_t status;

        status = put_string("0b");
        if (status != status::SUCCESS) [[unlikely]] {
            status = status::FAIL;
        }
        else {
            status = put_base(value, 2);
            if (status & status::flags::PARAM_B) {
                status = status::FAIL;
            }
        }

        return status;
    }


    /** 
     * @brief Outputs the address of pointer to the 
     *        terminal in hexadecimal.
     * 
     * @retval `status::FAIL`
     *          Unknown error.
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     * 
     * @param value address output
     */
    _API_  
    status_t 
    put_ptr(_IN_ const uint32_t value) noexcept {
        status_t status;

        status = put_string("0x");
        if (status != status::SUCCESS) [[unlikely]] {
            status = status::FAIL;
        }
        else {
            status = put_base(value, 16);
            if (status & status::flags::PARAM_B) {
                status = status::FAIL;
            }
        }

        return status;
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
     * 
     * @retval `status::FAIL`
     *          If the char puting fails.
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_ 
    status_t 
    put_char(_IN_ const char symbol) noexcept {
        status_t status;

        switch (symbol) {
        case '\r':
            status   = status::SUCCESS;
            cursor_x = 0;
            break;
        
        case '\b':
            status = handle_backspace();
            break;
        
        case '\t':
            status = handle_tab();
            break;
        
        case '\n':
            status = status::SUCCESS;
            new_line();
            break;

        case '\0':
            status = status::SUCCESS;
            break;
        
        default:
            status = handle_normal_char(symbol);
            break;
        }

        return status;
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
     * 
     * @retval `status::NULL_POINTER` 
     *          If `message` is a `nullptr`.
     * 
     * @retval `status::INVALID_PARAMETER`
     *          If `message` is empty.
     *
     * @retval `status::FAIL` 
     *          Unknown error.
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_ 
    status_t
    put_string(_IN_ const char* const message) noexcept {
        status_t status;
        uint32_t needed_lines;

        if (!message) [[unlikely]] {
            status = status::NULL_POINTER;
            goto cleanup;
        }

        if (message[0] == '\0') [[unlikely]] {
            status = status::INVALID_PARAMETER;
            goto cleanup;
        }

        status = calculate_needed_lines(needed_lines, message);
        if (status != status::SUCCESS) [[unlikely]] {
            if (status & status::flags::PARAM_B) {
                status ^= status::flags::PARAM_B;
            }

            goto cleanup;
        }
        
        if (needed_lines > 
            drivers::vga::TEXT_MODE_SCREEN_HEIGHT - cursor_y) [[unlikely]] {
            reset();
        }

        for (uint32_t current_char = 0; 
             message[current_char] != '\0';
             current_char++) [[likely]] {
            status = put_char(message[current_char]);
        }

    cleanup:
        return status;
    }
} // namespace stdlib::console
