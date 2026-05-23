/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
    
DESCRIPTION:
    This is the main shell class that provides the
    high-quality command interface for interacting with the kernel.

    The shell reads keyboard input, forwards characters to the command interpreter, 
    and manages the execution flow of user commands.
    
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
    because the compiler can see and optimize headers more easily than source files.
*/

#pragma once

#include "kernel/include/heap/linear_area.hpp"
#include "drivers/ps2/keyboardin/include/keyboard_input.hpp"
#include "drivers/vga/textmode/include/textmode.hpp"
#include "interpreter/interpreter.hpp"
#include <stddef.h>
#include <stdint.h>
#include <text_output.hpp>

namespace shell
{
    class Shell final {
    private:
        commands::Interpreter interpreter;

        void draw_user_cursor_with_color(drivers::vga::VGATextmodeColors foreground,
                                         drivers::vga::VGATextmodeColors background) 
                                         const noexcept;

        inline void draw_user_cursor() const noexcept {
            draw_user_cursor_with_color(
                drivers::vga::VGATextmodeColors::LIGHT_GREY,
                drivers::vga::VGATextmodeColors::LIGHT_GREY
            );
        }

        inline void erase_user_cursor() const noexcept {
            draw_user_cursor_with_color(
                drivers::vga::VGATextmodeColors::LIGHT_GREY,
                drivers::vga::VGATextmodeColors::BLACK
            );
        }

    public:
        void step() noexcept;

        Shell() noexcept;
        ~Shell() noexcept = default;
    };

    extern Shell shell;
} // namespace shell
