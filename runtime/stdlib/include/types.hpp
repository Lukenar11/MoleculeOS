/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This defines the fixed-width integer types used throughout the
    system.

    These typedefs provide consistent and predictable integer
    sizes across all compilation units, independent of compiler or
    platform-specific variations.

NOTES:
    The system does not rely on the host system's standard library, so
    this header replaces the functionality normally provided by the
    system's '<types.hpp>'.
*/

#pragma once

using int8_t  = signed char;  
using int16_t = signed short;
using int32_t = signed int;

using uint8_t  = unsigned char;
using uint16_t = unsigned short;
using uint32_t = unsigned int;
