/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file contains the IRQ handler table, 
    which maps IRQ numbers to their corresponding handler functions.

    The table is used by the "irq_common_handler" function to dispatch
    the appropriate handler for each IRQ.

NOTES:
*/

#pragma once

#include <drivers_api.hpp>

namespace kernel::irq
{
    struct IRQ_Handler_Entry final {
        uint8_t index = 0;
        void (*handler)(kernel::Register_Dump*);
    };

    inline constexpr IRQ_Handler_Entry irq_handler_table[] = {
        { 0, nullptr}, 
        { 1, +[](kernel::Register_Dump* reg_dump) -> void {
            drivers::ps2::Keyboard_Input::keyboard_irq_handler(reg_dump);
        }},
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
} // namespace kernel::irq
