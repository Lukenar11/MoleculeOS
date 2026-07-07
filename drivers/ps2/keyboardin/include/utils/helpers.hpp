/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
    
DESCRIPTION:
    This file defines the US QWERTY key mapping tables used by the
    PS/2 keyboard driver.
    
    The tables translate raw scancodes from the
    keyboard controller into ASCII characters, both for normal and
    Shift‑modified input.

    Additionally, this header declares the special scancode constants
    used to detect modifier keys such as Shift, CapsLock, and control
    signals like key release events.
    
NOTES:
    The mapping arrays must contain exactly 128 entries to match the
    valid scancode range of the PS/2 Set 1 protocol. 
        
    Any deviation in size or 
    ordering would result in incorrect character translation.
*/

#pragma once

#include <stdint.h>

namespace drivers::ps2
{
    constexpr char us_qwerty_std_key_mapping[128] = {
        0, 27, '1','2','3','4','5','6','7','8','9','0', '-','=', '\b',
        '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
        'a','s','d','f','g','h','j','k','l',';','\'','`', 0, '\\',
        'z','x','c','v','b','n','m',',','.','/', 0, '*', 0, ' ',
    };

    constexpr char us_qwerty_shift_key_mapping[128] = {
        0, 27, '!','@','#','$','%','^','&','*','(',')','_','+', '\b',
        '\t','Q','W','E','R','T','Y','U','I','O','P','{','}','\n', 0,
        'A','S','D','F','G','H','J','K','L',':','\"','~', 0, '|',
        'Z','X','C','V','B','N','M','<','>','?', 0, '*', 0, ' ',
    };

    enum class Special_Keyboard_Keys : uint8_t {
        LEFT_SHIFT          = 0x2A,
        RIGHT_SHIFT         = 0x36,
        KEYBOARD_RELEASE    = 0x80,
        LEFT_SHIFT_RELEASE  = 0xAA,
        RIGHT_SHIFT_RELEASE = 0xB6,
        CAPSLOCK            = 0x3A,
        CTRL                = 0x64,
        REBOOT_COMMAND      = 0xFE,
    };
} // namespace drivers::ps2
