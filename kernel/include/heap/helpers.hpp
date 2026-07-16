/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file exposes linker-defined variables for the start and end 
    of the kernel heap region.

    These variables represent raw memory addresses used by the heap allocator
    to determine the valid heap range.

NOTES:
    The internal definitions of "heap_start" and "heap_end" are located
    in the linker script (linker.ld).
*/

#pragma once

#include <stdint.h>

namespace kernel::heap
{
    extern "C" { 
        extern uint8_t heap_start;
        extern uint8_t heap_end;
    }
} // namespace kernel::heap
