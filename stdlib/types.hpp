/**
 * @brief Defines fixed width integer types and type limits constants.
 *
 * @copyright Copyright (c) 2026 Lukenar11 (Luke Matthes)
 * @license:  https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
 * @author    Lukenar11 (Luke Matthes)
 *
 * @note
 */


#pragma once


using int8_t  = signed char;
using int16_t = signed short;
using int32_t = signed int;

using int8_ptr  = int8_t*;
using int16_ptr = int16_t*;
using int32_ptr = int32_t*;

using int8_ref  = int8_t&;
using int16_ref = int16_t&;
using int32_ref = int32_t&;

using int8_pref  = int8_t*&;
using int16_pref = int16_t*&;
using int32_pref = int32_t*&;


using uint8_t  = unsigned char;
using uint16_t = unsigned short;
using uint32_t = unsigned int;

using uint8_ptr  = uint8_t*;
using uint16_ptr = uint16_t*;
using uint32_ptr = uint32_t*;

using uint8_ref  = uint8_t&;
using uint16_ref = uint16_t&;
using uint32_ref = uint32_t&;

using uint8_pref  = uint8_t*&;
using uint16_pref = uint16_t*&;
using uint32_pref = uint32_t*&;


#ifdef _ARCHITECTURE_I386_
using diff_t = signed int;
using size_t = unsigned int;
#else
#   error Architecture is not supported!
#endif


using char_t    = char;
using char_ptr  = char_t*;
using char_ref  = char_t&;
using char_pref = char_t*&;


using string_ptr = char_ptr;
using address_t  = size_t;
using any_ptr    = void*;


static_assert(sizeof(int8_t)  == 1, "byte size of 'int8_t' is not '1'!");
static_assert(sizeof(int16_t) == 2, "byte size of 'int16_t' is not '2'!");
static_assert(sizeof(int32_t) == 4, "byte size of 'int32_t' is not '4'!");

static_assert(sizeof(uint8_t)  == 1, "byte size of 'uint8_t' is not '1'!");
static_assert(sizeof(uint16_t) == 2, "byte size of 'uint16_t' is not '2'!");
static_assert(sizeof(uint32_t) == 4, "byte size of 'uint32_t' is not '4'!");

static_assert(sizeof(string_ptr) == 4, "byte size of 'string_ptr' is not '4'!");
static_assert(sizeof(any_ptr)    == 4, "byte size of 'any_ptr' is not '4'!");

#ifdef _ARCHITECTURE_I386_
static_assert(sizeof(diff_t) == 4, "byte size of 'diff_t' is not '4'!");
static_assert(sizeof(size_t) == 4, "byte size of 'size_t' is not '4'!");
#else
#   error Architecture is not supported!
#endif


namespace stdlib
{
    constexpr uint8_t UINT8_MAX   = 0xFF;
    constexpr uint16_t UINT16_MAX = 0xFF'FF;
    constexpr uint32_t UINT32_MAX = 0xFF'FF'FFFF;

    constexpr int8_t INT8_MIN   = 0x80;
    constexpr int16_t INT16_MIN = 0x80'00;
    constexpr int32_t INT32_MIN = 0x80'00'0000;

    constexpr int8_t INT8_MAX   = 0x7F;
    constexpr int16_t INT16_MAX = 0x7F'FF;
    constexpr int32_t INT32_MAX = 0x7F'FF'FFFF;
} // namespace stdlib
