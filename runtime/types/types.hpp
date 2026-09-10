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


constexpr uint8_t UINT8_MAX   = 0xFF;
constexpr uint16_t UINT16_MAX = 0xFF'FF;
constexpr uint32_t UINT32_MAX = 0xFF'FF'FF'FF;

constexpr int8_t INT8_MIN   = 0x80;
constexpr int16_t INT16_MIN = 0x80'00;
constexpr int32_t INT32_MIN = 0x80'00'00'00;

constexpr int8_t INT8_MAX   = 0x7F;
constexpr int16_t INT16_MAX = 0x7F'FF;
constexpr int32_t INT32_MAX = 0x7F'FF'FF'FF;


static_assert(sizeof(int8_t)  == 1, "sizeof(int8_t) != 1");
static_assert(sizeof(int16_t) == 2, "sizeof(int16_t) != 2");
static_assert(sizeof(int32_t) == 4, "sizeof(int32_t) != 4");

static_assert(sizeof(uint8_t)  == 1, "sizeof(int8_t) != 1");
static_assert(sizeof(uint16_t) == 2, "sizeof(int16_t) != 2");
static_assert(sizeof(uint32_t) == 4, "sizeof(int32_t) != 4");

static_assert(sizeof(void*) == 4, "sizeof(void*) != 4");
