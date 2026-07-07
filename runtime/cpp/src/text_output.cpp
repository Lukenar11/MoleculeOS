/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file contains the high-level text output system, 
    which provides formatted and unformatted print functions 
    on the VGA text mode driver.

    It provides character output, string output, 
    integer formatting, hexadecimal and binary print output, 
    pointer formatting, as well as a lightweight printf implementation 
    and supports setting various text colors.

    The "Text_Output" class manages its own cursor state, 
    color configuration, and line management, 
    allowing structured and readable output during runtime.
    
NOTES:
    Some methods are placed in the header 
    because they are so small that the compiler can inline them.
*/

#include <text_output.hpp>

namespace runtime
{
    void Text_Output::reset() noexcept {
        cursor_x = 0;
        cursor_y = 0;

        drivers::vga::Text_Mode::clear_screen(drivers::vga::Text_Mode_Colors::BLACK);
    }

    void Text_Output::set_text_color(const drivers::vga::Text_Mode_Colors& color,
                                     const drivers::vga::Text_Mode_Colors& background,
                                     const bool does_blink) 
                                     noexcept {
        cursor_color = drivers::vga::Text_Mode::make_color(color, background, does_blink);
    }

    void Text_Output::new_line() noexcept {
        cursor_x = 0;
        cursor_y++;

        if (cursor_y >= drivers::vga::TEXT_MODE_SCREEN_HEIGHT) [[unlikely]] {
            cursor_y = drivers::vga::TEXT_MODE_SCREEN_HEIGHT;
            reset();
        }
    }

    uint32_t Text_Output::calculate_needed_lines(const char* text) noexcept {
        const uint32_t null = 0;

        uint32_t needed_lines = 1;
        uint32_t x = cursor_x;

        while (*text) {
            const char symbol = *text++;
            if (symbol == '\n') {
                needed_lines++;
                x = null;
                continue;
            }

            x++;
            if (x >= drivers::vga::TEXT_MODE_SCREEN_WIDTH) {
                needed_lines++;
                x = null;
            }
        }

        return needed_lines;
    }

    void Text_Output::put_base(uint32_t value, const uint32_t base) noexcept {
        const uint32_t ten = 10;
        const char null_char = '0';
        const char a_char = 'A';

        if (value == 0) [[unlikely]] {
            put_char('0');
            return;
        }

        if (base < 2 || base > 16) [[unlikely]]
            return;
        
        runtime::Array<char, 32> buffer;
        uint32_t i = 0;
        while (value) {
            const uint8_t number = value % base;
            buffer[i++] = (number < ten) ? (null_char + number) : 
                                           (a_char + number - ten);
            value /= base;
        }

        while (i--)
            put_char(buffer[i]);
    }

    void Text_Output::put_char(const char symbol) noexcept {
        switch (symbol) {
        case '\r':
            cursor_x = 0;
            break;
            
        case '\b':
            if (cursor_x > 0) {
                cursor_x--;
                drivers::vga::Text_Mode::put_char_at(
                    ' ', 
                    cursor_color, 
                    cursor_x, 
                    cursor_y
                );
            }
            break;

        case '\t':
            for (uint32_t i = 0; i < 4; ++i) {
                drivers::vga::Text_Mode::put_char_at(
                    ' ', 
                    cursor_color, 
                    cursor_x, 
                    cursor_y
                );
                cursor_x++;

                if (cursor_x >= drivers::vga::TEXT_MODE_SCREEN_WIDTH) [[unlikely]]
                    new_line();
            }
            break;
    
        case '\n':
            new_line();
            break;

        default:
            drivers::vga::Text_Mode::put_char_at(
                symbol, 
                cursor_color, 
                cursor_x, 
                cursor_y
            );
            cursor_x++;

            if (cursor_x >= drivers::vga::TEXT_MODE_SCREEN_WIDTH) [[unlikely]]
                new_line();
            break;
        }
    }

    void Text_Output::put_string(const char* message, 
                                 const bool clear_screen_if_hight_limit_reached) 
                                 noexcept {
        const uint32_t needed_lines = calculate_needed_lines(message);
        const uint32_t remaining = drivers::vga::TEXT_MODE_SCREEN_HEIGHT - cursor_y;
        
        if (clear_screen_if_hight_limit_reached)
            if (needed_lines >= (remaining - 1)) [[unlikely]]
                reset();

        while (*message)
            put_char(*message++);
    }

    void Text_Output::put_int(int32_t value) noexcept {
        if (value < 0) {
            put_char('-');

            value = static_cast<uint32_t>(-(value + 1)) + 1;
            put_uint(value);

            return;
        }

        put_uint(value);
    }

    void Text_Output::put_uint(uint32_t value) noexcept {
        const uint32_t ten = 10;
        const char null_char = '0';

        if (value == 0) [[unlikely]] {
            put_char('0');
            return;
        }

        runtime::Array<char, 12> buffer;
        uint32_t i = 0;
        while (value) {
            buffer[i++] = null_char + (value % ten);
            value /= ten;
        }

        while (i--)
            put_char(buffer[i]);
    }

    void Text_Output::put_hex(const uint32_t value) noexcept {
        put_char('0'); 
        put_char('x');

        const uint32_t base = 16;
        put_base(value, base);
    }

    void Text_Output::put_bin(const uint32_t value) noexcept {
        put_char('0'); 
        put_char('b');

        const uint32_t base = 2;
        put_base(value, base);
    }

    void Text_Output::put_ptr(const uint32_t value) noexcept {
        put_char('0'); 
        put_char('x');

        const uint32_t base = 16;
        put_base(value, base);
    }
} // namespace runtime
