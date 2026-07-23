/**
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
        const char new_line = '\n';
        char key;
        while (drivers::ps2::Keyboard_Input::scancode_buffer.pop(key)) {
            erase_user_cursor();

            runtime::Text_Output::put_char(key);
            shell.step(key);

            draw_user_cursor();

            if (key == new_line) [[unlikely]]
                break;
        }
    }
}
