/*
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
    system's "<stdint.h>".
*/

#ifndef STDINT_H
#define STDINT_H

#ifdef __cplusplus
extern "C" {
#endif

typedef signed char  int8_t;
typedef signed short int16_t;
typedef signed int   int32_t;

typedef unsigned char  uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int   uint32_t;

typedef uint32_t size_t;

#ifdef __cplusplus
}
#endif

#endif  // STDINT_H
