/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file contains a helpers structure, 
    which is used for the 'irq_handler_table', 
    which contains all possible IRQ events.

NOTES:
*/

#pragma once

#include "utils/regs.hpp"
#include <stdint.h>

namespace kernel::idt::irq
{
    struct IRQ_Handler_Entry final {
        uint8_t index = 0;
        void (*handler)(kernel::Registers*);
    };
} // namespace kernel::irq
