/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
    
DESCRIPTION:
    This is the main shell class that provides the
    high-quality command interface for interacting with the kernel.

    The shell reads keyboard input, forwards characters to the 
    command interpreter, and manages the execution flow of user commands.
    
    The shell acts as a central, 
    user-visible system element that coordinates input processing, 
    command parsing, and output through the kernel's text output subsystem.
    
    The shell also provides a cursor and cursor blink for the user.
        
NOTES:
    The implementation is based on the PS/2 keyboard driver 
    for input and the interpreter class for command processing.

    The shell itself contains no parsing logic 
    and serves solely as a control layer and provides a cursor.

    Since "draw_user_cursor" and "erase_user_cursor" are quite simple, 
    I only declared them in the header so that the compiler can inline them, 
    because the 
    compiler can see and optimize headers more easily than source files.
*/

#pragma once

#include "shell/include/interpreter/interpreter.hpp"
#include <drivers_api.hpp>
#include <stdint.h>
#include <text_output.hpp>

namespace terminal
{
    class Terminal final {
    private:
        shell::interpreter::Interpreter interpreter;

        void draw_user_cursor_with_color(const drivers::vga::Text_Mode_Colors& foreground,
                                         const drivers::vga::Text_Mode_Colors& background) 
                                         const noexcept;

        inline constexpr void draw_user_cursor() const noexcept {
            draw_user_cursor_with_color(
                drivers::vga::Text_Mode_Colors::LIGHT_GREY,
                drivers::vga::Text_Mode_Colors::LIGHT_GREY
            );
        }

        inline constexpr void erase_user_cursor() const noexcept {
            draw_user_cursor_with_color(
                drivers::vga::Text_Mode_Colors::LIGHT_GREY,
                drivers::vga::Text_Mode_Colors::BLACK
            );
        }

    public:
        void step() noexcept;

        Terminal() noexcept;
        ~Terminal() noexcept = default;
    };

    extern Terminal terminal;
} // namespace terminal
