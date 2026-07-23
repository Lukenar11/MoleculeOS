/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This header declares the low-level routines for performing x86 port
    I/O operations. 
    
    These functions provide byte- and word-level access
    to hardware I/O ports and are used by drivers such as the PS/2
    controller, PIC, PIT, and VGA hardware.

    The routines are using the i386 instructions 'in', 'out' and 'insw', 'outsw'.

NOTES:
    The internal implementation is located in 'load_idt.asm'.
*/

#pragma once

#include <stdint.h>

namespace runtime
{
    extern "C" {
        uint8_t byte_input(uint16_t port);
        uint16_t word_input(uint16_t port);

        void word_input_stream(uint16_t port, 
                               uint32_t word_count,
                               uint16_t* buffer);
    
        void byte_output(uint16_t port, uint8_t value);
        void word_output(uint16_t port, uint16_t value);

        void word_output_stream(uint16_t port, 
                                uint32_t word_count,
                                uint16_t* buffer);
    }
} // namespace runtime
