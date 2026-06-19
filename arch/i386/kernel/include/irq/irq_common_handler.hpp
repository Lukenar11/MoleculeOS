/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    C interface for the general IRQ handler, which is called by "irq_common_stub".

    All hardware interruptstubs (irq_0–irq_15) 
    pass their register state to this function, 
    which performs uniform processing before the required
    End-of-Interrupt (EOI) signal is sent to the PIC.

NOTES:
    Since this function is called by an assembly routine, 
    it is declared as "extern "C"" to ensure compatibility.
*/

#pragma once

#include "arch/i386/kernel/include/utils/register_dump.hpp"
#include <port_io.h>

namespace kernel::irq
{
    extern "C"
    void irq_common_handler(Register_Dump* reg_dump);
} // namespace kernel::irq
