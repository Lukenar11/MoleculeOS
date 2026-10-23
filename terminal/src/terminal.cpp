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

    void Terminal::draw_user_cursor_with_color(const drivers::vga::Text_Mode_Color& foreground,
                                               const drivers::vga::Text_Mode_Color& background) 
                                               const noexcept {
        // uint32_t x = stdlib::console::get_cursor_x();
        // if (x >= drivers::vga::Text_Mode::screen_width()) [[unlikely]]
        //     x = drivers::vga::Text_Mode::screen_width() - 1;

        // uint32_t y = stdlib::console::get_cursor_y();
        // if (y >= drivers::vga::Text_Mode::screen_height()) [[unlikely]]
        //     y = drivers::vga::Text_Mode::screen_height() - 1;

        // stdlib::console::set_text_color(foreground, background);
        // 
        // const uint8_t color = stdlib::console::get_text_color();
        // drivers::vga::Text_Mode::set_symbol_at(x, y, color, ' ');
    }

    void Terminal::step() noexcept {
        const char new_line = '\n';
        char key;
        
        while (drivers::ps2::Keyboard_Input::pop_scancode_key(key) == status::SUCCESS) {
            erase_user_cursor();

            stdlib::console::put_char(key);
            shell.step(key);

            draw_user_cursor();

            if (key == new_line) [[unlikely]]
                break;
        }
    }
}
