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
    uint32_t Text_Output::convert_base_to_digits(_OUT_ stdlib::Array<char, 32>& buffer,
                                                 _IN_ uint32_t value,
                                                 _IN_  const uint32_t base) noexcept {
        const char null_char = '0';
        const char a_char    = 'A';
        const uint32_t ten   = 10;

        uint32_t i     = 0;
        uint8_t number = 0;

        while (value) [[likely]] {
            if (i >= buffer.size()) [[unlikely]] {
                break;
            }

            number = value % base;
            if (number < ten) {
                buffer[i++] = null_char + number;
            }
            else {
                buffer[i++] = a_char + number - ten;
            }

            value /= base;
        }

        return i;
    }

    void Text_Output::put_base(_IN_ uint32_t value, 
                               _IN_ const uint32_t base) noexcept {
        stdlib::Array<char, 32> buffer;
        uint32_t count;

        if (value == 0) [[unlikely]] {
            put_char('0');
            goto cleanup;
        }

        if (base < 2 || base > 16) [[unlikely]] {
            goto cleanup;
        }

        count = convert_base_to_digits(buffer, value, base);

        while (count--) [[likely]] {
            put_char(buffer[count]);
        }

    cleanup:
        return;
    }

    uint32_t Text_Output::convert_uint_to_digits(_OUT_ stdlib::Array<char, 12>& buffer,
                                                 _IN_  uint32_t value) noexcept {
        const char null_char = '0';
        const uint32_t ten   = 10;

        uint32_t i = 0;

        while (value) [[likely]] {
            if (i >= buffer.size()) [[unlikely]] {
                break;
            }

            buffer[i++] = null_char + (value % ten);
            value /= ten;
        }

        return i;
    }

    void Text_Output::handle_backspace() noexcept {
        if (cursor_x > 0) [[likely]] {
            cursor_x--;
            drivers::vga::Text_Mode::put_char_at(' ',
                                                 cursor_color,
                                                 cursor_x,
                                                 cursor_y);
        }
    }

    void Text_Output::handle_tab() noexcept {
        for (uint32_t i = 0; i < 4; ++i) [[likely]] {
            drivers::vga::Text_Mode::put_char_at(' ',
                                        cursor_color,
                                        cursor_x,
                                        cursor_y);
            cursor_x++;

            if (cursor_x >= drivers::vga::TEXT_MODE_SCREEN_WIDTH) [[unlikely]] {
                new_line();
            }
        }
    }

    void Text_Output::handle_normal_char(_IN_ const char symbol) noexcept {
        drivers::vga::Text_Mode::put_char_at(symbol,
                                             cursor_color,
                                             cursor_x,
                                             cursor_y);
        cursor_x++;

        if (cursor_x >= drivers::vga::TEXT_MODE_SCREEN_WIDTH) [[unlikely]] {
            new_line();
        }
    }

    uint32_t Text_Output::calculate_needed_lines(_IN_ const char* text) 
                                                 noexcept {
        const uint32_t null   = 0;
        uint32_t needed_lines = 1;
        uint32_t x            = cursor_x;

        char symbol;
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

        return needed_lines;
    }

    void Text_Output::new_line() noexcept {
        cursor_x = 0;
        cursor_y++;

        if (cursor_y >= drivers::vga::TEXT_MODE_SCREEN_HEIGHT) [[unlikely]] {
            reset();
        }
    }

    void Text_Output::put_int(_IN_ int32_t value) noexcept {
        if (value < 0) {
            put_char('-');

            value = static_cast<uint32_t>(-(value + 1)) + 1;
            put_uint(value);
        }
        else {
            put_uint(value);
        }
    }

    void Text_Output::put_uint(_IN_ uint32_t value) noexcept {
        uint32_t count;
        stdlib::Array<char, 12> buffer;
        
        if (value == 0) [[unlikely]] {
            put_char('0');
            goto cleanup;
        }

        count = convert_uint_to_digits(buffer, value);

        while (count--) [[likely]] {
            put_char(buffer[count]);
        }

    cleanup:
        return;
    }

    void Text_Output::put_char(_IN_ const char symbol) noexcept {
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
        
        default:
            handle_normal_char(symbol);
            break;
        }
    }

    void Text_Output::put_string(_IN_ const char* message, 
                                 _IN_ const bool premature_screen_clear) 
                                 noexcept {
        const uint32_t needed_lines = calculate_needed_lines(message);
        const uint32_t remaining    = drivers::vga::TEXT_MODE_SCREEN_HEIGHT - 
                                      cursor_y;
        
        if (premature_screen_clear) {
            if (needed_lines >= (remaining - 1)) [[unlikely]] {
                reset();
            }
        }

        while (*message) [[likely]] {
            put_char(*message++);
        }
    }
} // namespace stdlib
