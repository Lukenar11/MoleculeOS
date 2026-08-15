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

        static
        uint32_t convert_base_to_digits(_OUT_ stdlib::Array<char, 32>& buffer,
                                        _IN_ uint32_t value,
                                        _IN_  const uint32_t base) noexcept;

        static 
        void put_base(_IN_ uint32_t value, 
                      _IN_ const uint32_t base) noexcept;
            
        static
        uint32_t convert_uint_to_digits(_OUT_ stdlib::Array<char, 12>& buffer,
                                        _IN_  uint32_t value) noexcept;

        static void handle_backspace() noexcept;
        static void handle_tab() noexcept;
        static void handle_normal_char(_IN_ const char symbol) noexcept;

        static 
        uint32_t calculate_needed_lines(_IN_ const char* text) noexcept;

        static 
        void new_line() noexcept;

    public:            
        [[nodiscard]] static inline 
        uint32_t get_cursor_x() noexcept {
            return cursor_x;
        }

        [[nodiscard]] static inline 
        uint32_t get_cursor_y() noexcept {
            return cursor_y;
        }

        [[nodiscard]] static inline 
        uint8_t get_text_color() noexcept {
            return cursor_color;
        }

        static inline 
        void set_text_color(_IN_ const drivers::vga::Text_Mode_Colors& color,
                            _IN_ const drivers::vga::Text_Mode_Colors& background,
                            _IN_ const bool does_blink=false) 
                            noexcept {
            cursor_color = drivers::vga::Text_Mode::make_color(color, 
                                                               background, 
                                                               does_blink);
        }

        static inline constexpr 
        void set_cursor(_IN_ const uint32_t x, 
                        _IN_ const uint32_t y) noexcept {
            cursor_x = x;
            cursor_y = y;
        }

        static inline
        void reset() noexcept {
            using namespace drivers;

            cursor_x = 0;
            cursor_y = 0;

            vga::Text_Mode::clear_screen(vga::Text_Mode_Colors::BLACK);
        }

        static void put_int(_IN_  int32_t value) noexcept;
        static void put_uint(_IN_ uint32_t value) noexcept;

        static inline
        void put_hex(_IN_ const uint32_t value) noexcept {
            put_string("0x");
            put_base(value, 16);
        }

        static inline
        void put_bin(_IN_ const uint32_t value) noexcept {
            put_string("0b");
            put_base(value, 2);
        }

        static inline
        void put_ptr(_IN_ const uint32_t value) noexcept {
            put_string("0x");
            put_base(value, 16);
        }

        static 
        void put_char(_IN_ const char symbol) noexcept;

        static 
        void put_string(_IN_ const char* message, 
                        _IN_ const bool premature_screen_clear=true) 
                        noexcept;
    
        Text_Output() noexcept  = default;
        ~Text_Output() noexcept = default;
    };
} // namespace stdlib
