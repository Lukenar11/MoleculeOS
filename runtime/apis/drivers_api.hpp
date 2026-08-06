/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file defines a central API for all non-architecture-dependent drivers.

NOTES:
*/

#pragma once

#include <types.h>

namespace drivers::ata 
{
    enum class Driver_Operations : uint32_t;
}

#include "drivers/include/vga/text_mode.hpp"
#include "drivers/include/utils/vga_text_mode_helpers.hpp"

#include "drivers/include/ps2/keyboard_in.hpp"
#include "drivers/include/utils/ps2_keyboard_in_helpers.hpp"

#include "drivers/include/ata/pio.hpp"
#include "drivers/include/utils/ata_pio_helpers.hpp"
