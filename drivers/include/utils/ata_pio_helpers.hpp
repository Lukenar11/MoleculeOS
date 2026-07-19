/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This File contains a enum class for the 
    ATA-PIO-driver operation representation and commands.

NOTES:
*/

#pragma once

#include <stdint.h>

namespace drivers::ata
{
    enum class Driver_Operations : uint32_t {
        READ  = 0,
        WRITE = 1
    };
} // namespace drivers::ata
