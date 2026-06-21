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

#include "helpers.hpp"
