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
    void Text_Output::new_line() noexcept {
        using namespace drivers;

        cursor_x = 0;
        cursor_y++;

        if (cursor_y >= vga::TEXT_MODE_SCREEN_HEIGHT) [[unlikely]] {
            reset();
        }
    }

    uint32_t Text_Output::calculate_needed_lines(_IN_ const char* text) 
                                                 noexcept {
        using namespace drivers;

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
            if (x >= vga::TEXT_MODE_SCREEN_WIDTH) [[unlikely]] {
                needed_lines++;
                x = null;
            }
        }

        return needed_lines;
    }

    void Text_Output::put_base(_IN_ uint32_t value, 
                               _IN_ const uint32_t base) noexcept {
        const char null_char = '0';
        const char a_char    = 'A';
        const uint32_t ten   = 10;
        uint8_t number       = 0;
        uint32_t i           = 0;

        stdlib::Array<char, 32> buffer;
        
        if (value == 0) [[unlikely]] {
            put_char('0');
            goto cleanup;
        }

        if (base < 2 || base > 16) [[unlikely]] {
            goto cleanup;
        }

        while (value) [[likely]] {
            if (i >= buffer.size()) [[unlikely]] {
                break;
            }

            number      = value % base;
            buffer[i++] = (number < ten) ? 
                           null_char + number : 
                           a_char + number - ten;

            value /= base;
        }

        while (i--) [[likely]] {
            put_char(buffer[i]);
        }

    cleanup:
        return;
    }

    void Text_Output::put_char(_IN_ const char symbol) noexcept {
        using namespace drivers;

        switch (symbol) {
        case '\r':
            cursor_x = 0;
            break;
            
        case '\b':
            if (cursor_x > 0) [[likely]] {
                cursor_x--;
                vga::Text_Mode::put_char_at(' ', 
                                            cursor_color, 
                                            cursor_x, 
                                            cursor_y);
            }
            break;

        case '\t':
            for (uint32_t i = 0; i < 4; ++i) [[likely]] {
                vga::Text_Mode::put_char_at(' ', 
                                            cursor_color, 
                                            cursor_x, 
                                            cursor_y);
                cursor_x++;

                if (cursor_x >= vga::TEXT_MODE_SCREEN_WIDTH) [[unlikely]] {
                    new_line();
                }
            }
            break;
    
        case '\n':
            new_line();
            break;

        default:
            vga::Text_Mode::put_char_at(symbol, 
                                        cursor_color, 
                                        cursor_x, 
                                        cursor_y);
            cursor_x++;

            if (cursor_x >= vga::TEXT_MODE_SCREEN_WIDTH) [[unlikely]] {
                new_line();
            }
            break;
        }
    }

    void Text_Output::put_string(_IN_ const char* message, 
                                 _IN_ const bool premature_screen_clearing) 
                                 noexcept {
        using namespace drivers;

        const uint32_t needed_lines = calculate_needed_lines(message);
        const uint32_t remaining    = vga::TEXT_MODE_SCREEN_HEIGHT - cursor_y;
        
        if (premature_screen_clearing) {
            if (needed_lines >= (remaining - 1)) [[unlikely]] {
                reset();
            }
        }

        while (*message) [[likely]] {
            put_char(*message++);
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
        const char null_char = '0';
        const uint32_t ten   = 10;
        uint32_t i           = 0;

        stdlib::Array<char, 12> buffer;
        
        if (value == 0) [[unlikely]] {
            put_char('0');
            goto cleanup;
        }

        while (value) [[likely]] {
            if (i >= buffer.size()) [[unlikely]] {
                break;
            }

            buffer[i++] = null_char + (value % ten);
            value /= ten;
        }

        while (i--) [[likely]] {
            put_char(buffer[i]);
        }

    cleanup:
        return;
    }

    void Text_Output::put_hex(_IN_ const uint32_t value) noexcept {
        put_char('0'); 
        put_char('x');

        put_base(value, 16);
    }

    void Text_Output::put_bin(_IN_ const uint32_t value) noexcept {
        put_char('0'); 
        put_char('b');

        put_base(value, 2);
    }

    void Text_Output::put_ptr(_IN_ const uint32_t value) noexcept {
        put_char('0'); 
        put_char('x');

        put_base(value, 16);
    }
} // namespace stdlib
