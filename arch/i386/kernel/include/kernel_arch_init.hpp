/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This function is called from 'kernel_main' and 
    initializes all architecture specific components/systems
    such as the IDT and remap the PIC.

NOTES:
*/

#pragma once

#include "idt/idt.hpp"
#include "sys/interrupts.hpp"
#include <types.hpp>
#include <io.hpp>

namespace 
{
    struct PIC_Mapping final {
        uint16_t port = 0;
        uint8_t value = 0;
    };

    constexpr PIC_Mapping pic_mappings[] = {
        {.port=0x0020, .value=0x11},  // init master IPC
        {.port=0x00A0, .value=0x11},  // init slave IPC
        {.port=0x0021, .value=0x20},  // set master interrupt vector offsets
        {.port=0x00A1, .value=0x28},  // set slave interrupt vector offsets
        {.port=0x0021, .value=0x04},  // tell master where the slave is connected
        {.port=0x00A1, .value=0x02},  // tell slave its cascade identity
        {.port=0x0021, .value=0x01},  // set master 8086/88 mode
        {.port=0x00A1, .value=0x01},  // set slave 8086/88 mode
        {.port=0x0021, .value=0xFD}   // enable IRQ 1 (PS/2 controller)
    };
}

namespace kernel
{
    void kernel_arch_init() noexcept;
} // namespace kernel
