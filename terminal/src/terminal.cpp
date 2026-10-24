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
        draw_cursor();
    }

    void Terminal::draw_cursor() noexcept {
        stdlib::console::set_text_color(drivers::vga::Text_Mode_Color::LIGHT_GREY, 
                                        drivers::vga::Text_Mode_Color::LIGHT_GREY);

        stdlib::console::put_char(' ');

        stdlib::console::set_text_color(drivers::vga::Text_Mode_Color::LIGHT_GREY, 
                                        drivers::vga::Text_Mode_Color::BLACK);
    }

    void Terminal::step() noexcept {
        const char new_line = '\n';
        char key;
        
        while (drivers::ps2::Keyboard_Input::pop_scancode_key(key) == status::SUCCESS) {
            stdlib::console::put_char('\b');
            stdlib::console::put_char(key);

            draw_cursor();

            shell.step(key);

            if (key == new_line) [[unlikely]]
                break;
        }
    }
}
