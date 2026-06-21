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

#include "keyboard_input.hpp"

namespace drivers::ps2
{
    char Keyboard_Input::get_key() noexcept {
        const char null_terminator = '\0';

        if (!(runtime::byte_input(KEYBOARD_STATUS_PORT) & LOWEST_BIT))
            return null_terminator;
    
        const uint8_t scancode = runtime::byte_input(KEYBOARD_DATA_PORT);
    
        switch (scancode) {
        case static_cast<uint8_t>(Special_Keyboard_Keys::LEFT_SHIFT):
        case static_cast<uint8_t>(Special_Keyboard_Keys::RIGHT_SHIFT):
            shift_is_pressed = true;
            return null_terminator;
        
        case static_cast<uint8_t>(Special_Keyboard_Keys::LEFT_SHIFT_RELEASE):
        case static_cast<uint8_t>(Special_Keyboard_Keys::RIGHT_SHIFT_RELEASE):
            shift_is_pressed = false;
            return null_terminator;
        
        case static_cast<uint8_t>(Special_Keyboard_Keys::CAPSLOCK):
            capslock_is_enabled = !capslock_is_enabled;
            return null_terminator;
        }
    
        if (scancode & static_cast<uint8_t>(Special_Keyboard_Keys::KEYBOARD_RELEASE))
            return null_terminator;
    
        if (scancode >= ALOWED_SCANCODE_SIZE)
            return null_terminator;
    
        char character = (shift_is_pressed) 
                         ? us_qwerty_shift_key_mapping[scancode] 
                         : us_qwerty_std_key_mapping[scancode]; 

        const uint32_t thirty_two = 32;
        if (capslock_is_enabled) [[unlikely]] {
            if ((character >= 'a') && (character <= 'z'))
                character -= thirty_two;
            else if ((character >= 'A') && (character <= 'Z'))
                character += thirty_two;
        }

        return character;
    }

    // GLOBAL Keyboard-Input object
    Keyboard_Input keyboard_input;
} // namespace drivers::ps2
