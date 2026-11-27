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

using uint8_t  = unsigned char;
using uint16_t = unsigned short;
using uint32_t = unsigned int;

using char_t = char;

#ifdef _ARCHITECTURE_I386_
using diff_t = int32_t;
using size_t = uint32_t;
#else
#   error Architecture is not supported!
#endif

using address_t = size_t;


static_assert(sizeof(int8_t)  == 1, "'int8_t' is not 1 Byte long!");
static_assert(sizeof(int16_t) == 2, "'int16_t' is not 2 Bytes long!");
static_assert(sizeof(int32_t) == 4, "'int32_t' is not 4 Bytes long!");

static_assert(sizeof(uint8_t)  == 1, "'uint8_t' is not 1 Byte long!");
static_assert(sizeof(uint16_t) == 2, "'uint16_t' is not 2 Bytes long!");
static_assert(sizeof(uint32_t) == 4, "'uint32_t' is not 4 Bytes long!");

#ifdef _ARCHITECTURE_I386_
static_assert(sizeof(diff_t) == 4, "'diff_t' is not 2 Bytes long!");
static_assert(sizeof(size_t) == 4, "'size_t' is not 4 Bytes long!");
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
