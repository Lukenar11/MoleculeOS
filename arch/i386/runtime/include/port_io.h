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

    The routines are using the i386 instructions "in" and "out".

NOTES:
    The internal implementation is located in "load_idt.asm".

    This header is written in C rather than C++
    to ensure maximum compatibility, 
    as Assembly provides a C interface, but not a C++ interface.
*/

#ifndef PORT_IO_H
#define PORT_IO_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint8_t inb(uint16_t port);
void outb(uint16_t port, uint8_t value);
uint16_t inw(uint16_t port);
void outw(uint16_t port, uint16_t value);

#ifdef __cplusplus
}
#endif

#endif // PORT_IO_H
