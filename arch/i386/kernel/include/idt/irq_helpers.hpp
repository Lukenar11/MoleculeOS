/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    C interface for the general IRQ handler, 
    which is called by "irq_common_stub".

    All hardware interrupt stubs (irq_0–irq_15) 
    pass their register state to this function, 
    which performs uniform processing before the required
    End-of-Interrupt (EOI) signal is sent to the PIC.

NOTES:
    Since this function is called by an assembly routine, 
    it is declared as "extern "C"" to ensure compatibility.
*/

#pragma once

#include "utils/register_dump.hpp"
#include <drivers_api.hpp>
#include <io_arch_api.hpp>

namespace
{
    constexpr uint8_t EOF                  = 0x20;
    constexpr uint8_t MIN_INTERRUPT_VECTOR = 0x28;
    constexpr uint8_t MAX_INTERRUPT_VECTOR = 0x2F;
    constexpr uint8_t MAX_IRQ_EVENT_NUM    = 15;

    constexpr uint16_t SLAVE_PIC_CMD_PORT  = 0xA0;
    constexpr uint16_t MASTER_PIC_CMD_PORT = 0x20;

    struct IRQ_Handler_Entry final {
        uint8_t index                           = 0;
        void (*handler)(kernel::Register_Dump*) = nullptr;
    };

    inline constexpr IRQ_Handler_Entry irq_handler_table[] = {
        { 0, nullptr}, 
        { 1, drivers::ps2::Keyboard_Input::keyboard_irq_handler},
        { 2, nullptr}, 
        { 3, nullptr}, 
        { 4, nullptr},
        { 5, nullptr}, 
        { 6, nullptr},
        { 7, nullptr}, 
        { 8, nullptr},
        { 9, nullptr}, 
        {10, nullptr}, 
        {11, nullptr},
        {12, nullptr}, 
        {13, nullptr}, 
        {14, nullptr},
        {15, nullptr}
    };
}

namespace kernel::irq
{
    extern "C"
    void irq_common_handler(Register_Dump* reg_dump);
} // namespace kernel::irq
