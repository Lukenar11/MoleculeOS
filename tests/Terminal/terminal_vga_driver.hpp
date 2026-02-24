#pragma once

#include "Terminal/include/Drivers/VGA.hpp"

void test_vga_driver() {

    const char text_message[] = "Hello from C++ kernel!";
    terminal::drivers::VGA vga;

    // Clear Screen
    vga.clear_screen(uint8_t(terminal::drivers::VGAColors::BLACK));

    // puts(text_message);
    for (int x = 0; x < (sizeof(text_message) / sizeof(char)); x++)
        vga.put_char_at(
            text_message[x], 
            uint8_t(terminal::drivers::VGAColors::GREEN), 
            x, 18
        );

    // char Color-Test
    for (int x = 0; x < 16; x++)
        for (int y = 0; y < 16; y++)
            vga.put_char_at(
                    '0', vga.make_color(
                        terminal::drivers::VGAColors(x),
                        terminal::drivers::VGAColors(y)
                    ),
                    x, y
                );
}
