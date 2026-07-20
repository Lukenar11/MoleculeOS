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
    user-visible sys element that coordinates input processing, 
    command parsing, and output through the kernel's text output subsys.

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

#include "terminal.hpp"

namespace terminal 
{
    Terminal::Terminal() noexcept {
        shell::commands::info();
        draw_user_cursor();
    }

    void Terminal::draw_user_cursor_with_color(const drivers::vga::Text_Mode_Colors& foreground,
                                               const drivers::vga::Text_Mode_Colors& background) 
                                               const noexcept {
        uint32_t x = runtime::Text_Output::get_cursor_x();
        if (x >= drivers::vga::TEXT_MODE_SCREEN_WIDTH) [[unlikely]]
            x = drivers::vga::TEXT_MODE_SCREEN_WIDTH - 1;

        uint32_t y = runtime::Text_Output::get_cursor_y();
        if (y >= drivers::vga::TEXT_MODE_SCREEN_HEIGHT) [[unlikely]]
            y = drivers::vga::TEXT_MODE_SCREEN_HEIGHT - 1;

        runtime::Text_Output::set_text_color(foreground, background);
        
        const uint8_t color = runtime::Text_Output::get_text_color();
        drivers::vga::Text_Mode::put_char_at(' ', color, x, y);
    }

    void Terminal::step() noexcept {
        char key;
        while (drivers::ps2::Keyboard_Input::scancode_buffer.pop(key)) {
            erase_user_cursor();

            runtime::Text_Output::put_char(key);
            shell.step(key);

            draw_user_cursor();

            if (key == '\n')
                break;
        }
    }
}
