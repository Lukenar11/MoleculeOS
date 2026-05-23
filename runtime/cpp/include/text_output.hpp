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

#pragma once

#include "drivers/vga/textmode/include/textmode.hpp"
#include <stdint.h>
#include <stdarg.h>
#include <array.hpp>

namespace runtime 
{
    class TextOutput final {
    private:
        uint32_t cursor_x = NULL;
        uint32_t cursor_y = NULL;
        
        uint8_t cursor_color = drivers::vga::texmode.make_color(
            drivers::vga::VGATextmodeColors::LIGHT_GREY, 
            drivers::vga::VGATextmodeColors::BLACK
        );

        void new_line() noexcept;
        void put_base(uint32_t value, const uint32_t base) noexcept;

        uint32_t calculate_needed_lines(const char* text) noexcept;
            
    public:
        void reset() noexcept;
        void set_text_color(const drivers::vga::VGATextmodeColors& color,
                            const drivers::vga::VGATextmodeColors& background) 
                            noexcept;
        
        void put_char(const char symbol) noexcept;
        void put_string(const char* message) noexcept;
            
        void put_int(int32_t value) noexcept;
        void put_uint(uint32_t value) noexcept;
            
        void put_hex(const uint32_t value) noexcept;
        void put_bin(const uint32_t value) noexcept;
            
        void put_ptr(const uintptr_t value) noexcept;

        inline constexpr uint32_t get_cursor_x() const noexcept { return cursor_x; }
        inline constexpr uint32_t get_cursor_y() const noexcept { return cursor_y; }
        inline constexpr uint8_t get_text_color() const noexcept { return cursor_color; }

        inline constexpr void set_cursor(const uint32_t x, const uint32_t y) noexcept {
            cursor_x = x;
            cursor_y = y;
        }
    
        TextOutput() noexcept = default;
        ~TextOutput() noexcept = default;
    };

    // GLOBAL Screen-Output objekt
    extern TextOutput text_output;
} // namespace runtime
