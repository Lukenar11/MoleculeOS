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
    This implementation is intentionally minimal and written in C to
    ensure compatibility with both C and C++ components of the system,
    especially those interacting with Assembly routines.

    The system does not rely on the host system's standard library, so
    this header replaces the functionality normally provided by the
    system's '<types.h>'.
*/

#include <types.h>
