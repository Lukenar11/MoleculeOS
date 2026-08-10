/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file exposes the low-level CPU interrupt enabling routine.
    The routine uses the i386 instruction "sti" to disable interrupts.

NOTES:
    The internal implementation is located in 'interrupts.hpp'.
*/

#pragma once

#include <types.hpp>

namespace kernel::sys
{
    extern "C" {
        void enable_interrupts();
        void trigger_interrupt(uint32_t);
    }
} // namespace kernel::sys
