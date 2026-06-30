/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    The function "kernel_arch_init" is calles from "kernel_main" and 
    initializes core subsystems such as the IDT and remap the PIC.

NOTES:
*/

#pragma once

#include "idt/idt.hpp"
#include "system/enable_interrupts.hpp"
#include <stdint.h>
#include <io_arch_api.hpp>

namespace kernel
{
    void kernel_arch_init() noexcept;
} // namespace kernel
