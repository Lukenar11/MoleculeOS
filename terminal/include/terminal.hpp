/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
    
DESCRIPTION:
    This class is the main terminal class, 
    which delivers a high-level UI for the user and uses the shell internally.

    The terminal draws ans updates the user cursor and 
    gives the shell char per char user inputs.
        
NOTES:
    Some methods are placed in the header 
    because they are so small that the compiler can inline them.
*/

#pragma once

#include <shell_api.hpp>
#include <drivers_api.hpp>
#include <stdint.h>
#include <text_output.hpp>

namespace terminal
{
    class Terminal final {
    private:
        shell::Shell shell;

        void draw_user_cursor_with_color(const drivers::vga::Text_Mode_Colors& foreground,
                                         const drivers::vga::Text_Mode_Colors& background) 
                                         const noexcept;

        inline constexpr void draw_user_cursor() const noexcept {
            draw_user_cursor_with_color(drivers::vga::Text_Mode_Colors::LIGHT_GREY,
                                        drivers::vga::Text_Mode_Colors::LIGHT_GREY);
        }

        inline constexpr void erase_user_cursor() const noexcept {
            draw_user_cursor_with_color(drivers::vga::Text_Mode_Colors::LIGHT_GREY,
                                        drivers::vga::Text_Mode_Colors::BLACK);
        }

    public:
        void step() noexcept;

        Terminal() noexcept;
        ~Terminal() noexcept = default;
    };
} // namespace terminal
