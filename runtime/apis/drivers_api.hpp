/* 
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file defines a central API for all non-architecture-dependent drivers.

NOTES:
*/

#pragma once

#include "drivers/vga/textmode/include/text_mode.hpp"
#include "drivers/vga/textmode/include/utils/helpers.hpp"
#include "drivers/ps2/keyboardin/include/keyboard_input.hpp"
#include "drivers/ps2/keyboardin/include/utils/helpers.hpp"
#include "drivers/ata/pio/include/programmable_input_output.hpp"
#include "drivers/ata/pio/include/utils/helpers.hpp"