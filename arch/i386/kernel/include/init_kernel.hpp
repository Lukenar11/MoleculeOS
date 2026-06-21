/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file contains the kernel entry point "kernel_main," which
    is called directly after the bootloader hands control over to the kernel.

    The function initializes core subsystems such as the IDT with the function init_kernel,
    performs a simple interrupt- and endless-loop-driven scheduling, and starts the shell.

NOTES:
    This "kernel_main" function must never return. If execution reaches the end of
    "kernel_main," a kernel panic is triggered to prevent undefined behavior.

    Since this function is called by an assembly routine,
    it is declared as "extern C" to ensure compatibility.
*/

#pragma once

#include "idt/idt.hpp"
#include "system/enable_interrupts.hpp"
#include "terminal.hpp"
#include "system/sleep.hpp"
#include "system/panic.hpp"
#include <stdint.h>

namespace
{
    struct PIC_Mapping final {
        uint16_t port = 0;
        uint8_t value = 0;
    };
}

namespace kernel
{
    void remap_pic() noexcept;
    void init_kernel() noexcept;
}
