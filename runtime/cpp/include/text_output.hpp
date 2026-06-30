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

#pragma once

#include <drivers_api.hpp>
#include <stdint.h>
#include <array.hpp>

namespace runtime 
{
    class Text_Output final {
    private:
        uint32_t cursor_x = 0;
        uint32_t cursor_y = 0;
        
        const bool does_blink = false;
        uint8_t cursor_color = drivers::vga::text_mode.make_color(
            drivers::vga::Text_Mode_Colors::LIGHT_GREY, 
            drivers::vga::Text_Mode_Colors::BLACK,
            does_blink
        );

        void new_line() noexcept;
        void put_base(uint32_t value, const uint32_t base) noexcept;

        uint32_t calculate_needed_lines(const char* text) noexcept;
            
    public:
        void reset() noexcept;
        void set_text_color(const drivers::vga::Text_Mode_Colors& color,
                            const drivers::vga::Text_Mode_Colors& background,
                            const bool does_blink=false) 
                            noexcept;
        
        void put_char(const char symbol) noexcept;
        void put_string(const char* message, 
                        const bool clear_screen_if_hight_limit_reached=true)
                        noexcept;
            
        void put_int(int32_t value) noexcept;
        void put_uint(uint32_t value) noexcept;
            
        void put_hex(const uint32_t value) noexcept;
        void put_bin(const uint32_t value) noexcept;
            
        void put_ptr(const uint32_t value) noexcept;

        inline constexpr uint32_t get_cursor_x() const noexcept { 
            return cursor_x; 
        }

        inline constexpr uint32_t get_cursor_y() const noexcept { 
            return cursor_y; 
        }

        inline constexpr uint8_t get_text_color() const noexcept { 
            return cursor_color; 
        }

        inline constexpr void set_cursor(const uint32_t x, 
                                         const uint32_t y) noexcept {
            cursor_x = x;
            cursor_y = y;
        }
    
        Text_Output() noexcept = default;
        ~Text_Output() noexcept = default;
    };

    extern Text_Output text_output;
} // namespace runtime
