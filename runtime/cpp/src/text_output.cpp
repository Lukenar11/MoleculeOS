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
*/

#include "text_output.hpp"

namespace runtime
{
    void Text_Output::reset() noexcept {
        const uint32_t null = 0;

        cursor_x = null;
        cursor_y = null;

        drivers::vga::texmode.clear_screen(drivers::vga::VGA_Textmode_Colors::BLACK);
    }

    void Text_Output::set_text_color(const drivers::vga::VGA_Textmode_Colors& color,
                                     const drivers::vga::VGA_Textmode_Colors& background,
                                     const bool does_blink) 
                                     noexcept {
        cursor_color = drivers::vga::texmode.make_color(color, background, does_blink);
    }

    void Text_Output::new_line() noexcept {
        cursor_x = 0;
        cursor_y++;

        if (cursor_y >= drivers::vga::VGA_TEXMODE_SCREEN_HEIGHT) [[unlikely]] {
            cursor_y = drivers::vga::VGA_TEXMODE_SCREEN_HEIGHT;
            reset();
        }
    }

    uint32_t Text_Output::calculate_needed_lines(const char* text) noexcept {
        const uint32_t null = 0;

        uint32_t needed_lines = 1;
        uint32_t x = cursor_x;

        while (*text) [[likely]] {
            const char symbol = *text++;
            if (symbol == '\n') {
                needed_lines++;
                x = null;
                continue;
            }

            x++;
            if (x >= drivers::vga::VGA_TEXMODE_SCREEN_WIDTH) {
                needed_lines++;
                x = null;
            }
        }

        return needed_lines;
    }

    void Text_Output::put_base(uint32_t value, const uint32_t base) noexcept {
        const uint32_t null = 0;
        const uint32_t ten = 10;
        const char char_null = '0';
        const char char_a = 'A';

        if (value == null) [[unlikely]] {
            put_char('0');
            return;
        }

        if (base < 2 || base > 16) [[unlikely]]
            return;
        
        runtime::Array<char, 32> buffer;
        uint32_t i = null;
        while (value) [[likely]] {
            const uint8_t number = value % base;
            buffer[i++] = (number < ten) ? (char_null + number) : (char_a + number - ten);
            value /= base;
        }

        while (i--) [[likely]]
            put_char(buffer[i]);
    }

    void Text_Output::put_char(const char symbol) noexcept {
        const uint32_t null = 0;
        switch (symbol) {
        case '\r':
            cursor_x = null;
            break;
            
        case '\b':
            if (cursor_x > null) {
                cursor_x--;
                drivers::vga::texmode.put_char_at(' ', cursor_color, cursor_x, cursor_y);
            }
            break;

        case '\t':
            for (uint32_t i = null; i < 4; ++i) {
                drivers::vga::texmode.put_char_at(' ', cursor_color, cursor_x, cursor_y);
                cursor_x++;

                if (cursor_x >= drivers::vga::VGA_TEXMODE_SCREEN_WIDTH) [[unlikely]]
                    new_line();
            }
            break;
    
        case '\n':
            new_line();
            break;

        default:
            drivers::vga::texmode.put_char_at(symbol, cursor_color, cursor_x, cursor_y);
            cursor_x++;

            if (cursor_x >= drivers::vga::VGA_TEXMODE_SCREEN_WIDTH) [[unlikely]]
                new_line();
            break;
        }
    }

    void Text_Output::put_string(const char* message) noexcept {
        const uint32_t needed_lines = calculate_needed_lines(message);
        const uint32_t remaining = drivers::vga::VGA_TEXMODE_SCREEN_HEIGHT - cursor_y;
        if (needed_lines >= remaining) [[unlikely]]
            reset();

        while (*message) [[likely]]
            put_char(*message++);
    }

    void Text_Output::put_int(int32_t value) noexcept {
        const uint32_t one = 1;
        if (value < 0) [[unlikely]] {
            put_char('-');

            value = static_cast<uint32_t>(-(value + one)) + one;
            put_uint(value);

            return;
        }

        put_uint(value);
    }

    void Text_Output::put_uint(uint32_t value) noexcept {
        const uint32_t null = 0;
        const uint32_t ten = 10;
        const char null_char = '0';

        if (value == null) [[unlikely]] {
            put_char('0');
            return;
        }

        runtime::Array<char, 12> buffer;
        uint32_t i = null;
        while (value) [[likely]] {
            buffer[i++] = null_char + (value % ten);
            value /= ten;
        }

        while (i--) [[likely]]
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

    void Text_Output::put_ptr(const uintptr_t value) noexcept {
        put_char('0'); 
        put_char('x');

        const uint32_t base = 16;
        put_base(value, base);
    }

    // GLOBAL Screen-Output objekt
    Text_Output text_output;
} // namespace runtime
