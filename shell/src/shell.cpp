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
*/

#include "shell.hpp"

namespace shell 
{
    Shell::Shell() noexcept {
        commands::info();
        draw_user_cursor();
    }

    void Shell::draw_user_cursor() const noexcept {
        uint32_t x = runtime::text_output.get_cursor_x();
        if (x >= drivers::vga::VGA_TEXMODE_SCREEN_WIDTH)
            x = drivers::vga::VGA_TEXMODE_SCREEN_WIDTH - 1;

        uint32_t y = runtime::text_output.get_cursor_y();
        if (y >= drivers::vga::VGA_TEXMODE_SCREEN_HEIGHT)
            y = drivers::vga::VGA_TEXMODE_SCREEN_HEIGHT - 1;

        runtime::text_output.set_text_color(
            drivers::vga::VGATextmodeColors::LIGHT_GREY, 
            drivers::vga::VGATextmodeColors::LIGHT_GREY
        );

        const uint8_t user_cursor_color = runtime::text_output.get_text_color();
        drivers::vga::texmode.put_char_at(' ', user_cursor_color, x, y);
    }

    void Shell::erase_user_cursor() const noexcept {
        uint32_t x = runtime::text_output.get_cursor_x();
        if (x >= drivers::vga::VGA_TEXMODE_SCREEN_WIDTH)
            x = drivers::vga::VGA_TEXMODE_SCREEN_WIDTH - 1;

        uint32_t y = runtime::text_output.get_cursor_y();
        if (y >= drivers::vga::VGA_TEXMODE_SCREEN_HEIGHT)
            y = drivers::vga::VGA_TEXMODE_SCREEN_HEIGHT - 1;

        runtime::text_output.set_text_color(
            drivers::vga::VGATextmodeColors::LIGHT_GREY, 
            drivers::vga::VGATextmodeColors::BLACK
        );

        const uint8_t user_cursor_color = runtime::text_output.get_text_color();
        drivers::vga::texmode.put_char_at(' ', user_cursor_color, x, y);
    }

    void Shell::step() noexcept {
        while (drivers::ps2::keyboard_input.has_pending_scancode()) {
            const char key = drivers::ps2::keyboard_input.get_key();
            if (!key)
                continue;

            erase_user_cursor();

            runtime::text_output.put_char(key);
            interpreter.step(key);

            draw_user_cursor();

            if (key == '\n')
                break;
        }
    }
}
