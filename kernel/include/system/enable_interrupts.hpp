/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file exposes the low-level CPU interrupt enebling routine.
    The routine uses the i386 instruction "sti" to disable interrupts.

NOTES:
    The internal implementation is located in "enable_interrupts.asm".
*/

#pragma once

namespace kernel::system
{
    extern "C"
    void enable_interrupts();
} // namespace kernel::system
