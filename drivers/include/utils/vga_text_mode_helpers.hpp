/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This is a VGA text mode color enumeration and screen
    dimension constants used by the VGA text mode driver. 
        
    The color values correspond directly to the 4‑bit VGA attribute format used
    for foreground and background colors.

    The constants for screen width and height reflect the standard
    80×25 VGA text mode layout and are used throughout the driver for
    bounds checking and screen operations.

NOTES:
*/

#pragma once

#include <types.h>

namespace drivers::vga 
{
    constexpr uint32_t TEXT_MODE_SCREEN_WIDTH  = 80;
    constexpr uint32_t TEXT_MODE_SCREEN_HEIGHT = 25;

    enum class Text_Mode_Colors : uint8_t {
        BLACK         = 0x00,
        BLUE          = 0x01,
        GREEN         = 0x02,
        CYAN          = 0x03,
        RED           = 0x04,
        MAGENTA       = 0x05,
        BROWN         = 0x06,
        LIGHT_GREY    = 0x07,
        DARK_GREY     = 0x08,
        LIGHT_BLU     = 0x09,
        LIGHT_GREEN   = 0x0A,
        LIGHT_CYAN    = 0x0B,
        LIGHT_RED     = 0x0C,
        LIGHT_MAGENTA = 0x0D,
        YELLOW        = 0x0E,
        WHITE         = 0x0F
    };
} // namespace drivers::vga
