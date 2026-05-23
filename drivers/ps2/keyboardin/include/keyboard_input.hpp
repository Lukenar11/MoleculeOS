/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This is a PS/2 keyboard input driver, for reading translated keypresses from the
    keyboard controller (i8042).

    The driver reads raw scancodes from the data port, applies modifier
    state such as Shift and CapsLock, and returns the resulting ASCII
    character to other components like the shell.

NOTES:
    The function "has_pending_scancode" is intentionally in the header because 
    the compiler can see and optimize header files better than source files; 
    since this function is so small, the compiler can therefore probably inline it.
*/

#pragma once

#include "utils/helpers.hpp"
#include <stdint.h>
#include <port_io.h>

namespace drivers::ps2 
{
    class KeyboardInput final {
    private:
        static constexpr uint8_t LOWEST_BIT = 0x01;
            static constexpr uint8_t ALOWED_SCANCODE_SIZE = 128;

        static constexpr uint16_t KEYBOARD_STATUS_PORT = 0x64;
        static constexpr uint16_t KEYBOARD_DATA_PORT = 0x60;
    
        bool shift_is_pressed = false;
        bool capslock_is_enabled = false;

    public:
        inline bool has_pending_scancode() const noexcept {
            return inb(KEYBOARD_STATUS_PORT) & LOWEST_BIT;
        }

        char get_key() noexcept;
    
        KeyboardInput() noexcept = default;
        ~KeyboardInput() noexcept = default;
    };

    // GLOBAL Keyboard-Input object
    extern KeyboardInput keyboard_input;
} // namespace drivers::ps2
