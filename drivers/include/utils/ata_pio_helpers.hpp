/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This File contains a 'enum class' for the 
    ATA-PIO-driver operation and commands representation.

NOTES:
*/

#pragma once

#include <types.h>

namespace drivers::ata
{
    inline constexpr uint32_t SECTOR_SIZE = 512;
    inline constexpr uint32_t SECTOR_WORD_SIZE = SECTOR_SIZE / sizeof(uint16_t);

    enum class Driver_Operations : uint32_t {
        READ  = 0,
        WRITE = 1
    };
} // namespace drivers::ata
