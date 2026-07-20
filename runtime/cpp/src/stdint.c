/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This defines the fixed-width integer types used throughout the
    sys.

    These typedefs provide consistent and predictable integer
    sizes across all compilation units, independent of compiler or
    platform-specific variations.

NOTES:
    This implementation is intentionally minimal and written in C to
    ensure compatibility with both C and C++ components of the sys,
    especially those interacting with Assembly routines.

    The sys does not rely on the host sys's standard library, so
    this header replaces the functionality normally provided by the
    sys's "<stdint.h>".
*/

#include <stdint.h>
