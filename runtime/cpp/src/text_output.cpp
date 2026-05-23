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

    The "TextOutput" class manages its own cursor state, 
    color configuration, and line management, 
    allowing structured and readable output during runtime.
    
NOTES:
*/

#include "text_output.hpp"

namespace runtime
{
    void TextOutput::reset() noexcept {
        cursor_x = NULL;
        cursor_y = NULL;

        drivers::vga::texmode.clear_screen(drivers::vga::VGA_Textmode_Colors::BLACK);
    }

    void TextOutput::set_text_color(const drivers::vga::VGA_Textmode_Colors& color,
                                    const drivers::vga::VGA_Textmode_Colors& background) 
                                    noexcept {
        cursor_color = drivers::vga::texmode.make_color(color, background);
    }

    void TextOutput::new_line() noexcept {
        cursor_x = NULL;
        cursor_y++;

        if (cursor_y >= drivers::vga::VGA_TEXMODE_SCREEN_HEIGHT) [[unlikely]] {
            cursor_y = drivers::vga::VGA_TEXMODE_SCREEN_HEIGHT;
            reset();
        }
    }

    uint32_t TextOutput::calculate_needed_lines(const char* text) noexcept {
        uint32_t needed_lines = 1;
        uint32_t x = cursor_x;

        while (*text) {
            const char symbol = *text++;
            if (symbol == '\n') {
                needed_lines++;
                x = NULL;
                continue;
            }

            x++;
            if (x >= drivers::vga::VGA_TEXMODE_SCREEN_WIDTH) {
                needed_lines++;
                x = NULL;
            }
        }

        return needed_lines;
    }

    void TextOutput::put_base(uint32_t value, const uint32_t base) noexcept {
        if (value == NULL) [[unlikely]] {
            put_char('0');
            return;
        }

        if (base < 2 || base > 16) [[unlikely]]
            return;

        runtime::Array<char, 32> buffer;
        uint32_t i = NULL;

        while (value) {
            const uint32_t ten = 10;

            const uint8_t number = value % base;
            buffer[i++] = (number < ten) ? ('0' + number) : ('A' + number - ten);
            value /= base;
        }

        while (i--)
            put_char(buffer[i]);
    }

    void TextOutput::put_char(const char symbol) noexcept {
        switch (symbol) {
        case '\r':
            cursor_x = NULL;
            break;
            
        case '\b':
            if (cursor_x > NULL) {
                cursor_x--;
                drivers::vga::texmode.put_char_at(' ', cursor_color, cursor_x, cursor_y);
            }
            break;

        case '\t':
            for (uint32_t i = NULL; i < 4; ++i) {
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

    void TextOutput::put_string(const char* message) noexcept {
        const uint32_t needed_lines = calculate_needed_lines(message);
        const uint32_t remaining = drivers::vga::VGA_TEXMODE_SCREEN_HEIGHT - cursor_y;
        if (needed_lines >= remaining) [[unlikely]]
            reset();

        while (*message)
            put_char(*message++);
    }

    void TextOutput::put_int(int32_t value) noexcept {
        if (value < NULL) [[unlikely]] {
            put_char('-');

            const uint32_t one = 1;
            value = static_cast<uint32_t>(-(value + one)) + one;
            put_uint(value);

            return;
        }

        put_uint(value);
    }

    void TextOutput::put_uint(uint32_t value) noexcept {
        if (value == NULL) [[unlikely]] {
            put_char('0');
            return;
        }

        runtime::Array<char, 12> buffer;
        uint32_t i = NULL;

        while (value) {
            buffer[i++] = '0' + (value % 10);
            value /= 10;
        }

        while (i--)
            put_char(buffer[i]);
    }

    void TextOutput::put_hex(const uint32_t value) noexcept {
        put_char('0'); 
        put_char('x');

        const uint32_t base = 16;
        put_base(value, base);
    }

    void TextOutput::put_bin(const uint32_t value) noexcept {
        put_char('0'); 
        put_char('b');

        const uint32_t base = 2;
        put_base(value, base);
    }

    void TextOutput::put_ptr(const uintptr_t value) noexcept {
        put_char('0'); 
        put_char('x');

        const uint32_t base = 16;
        put_base(value, base);
    }

    // GLOBAL Screen-Output objekt
    TextOutput text_output;
} // namespace runtime
