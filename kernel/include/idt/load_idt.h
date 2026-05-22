/*
    LICENSE:
    	Copyright (c) 2026 Lukenar11 (Luke Matthes)
    	MIT Licensed
    	https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
    
    DESCRIPTION:
    	This is a simple Assembly routine for loading the IDT.
    
    	This routine uses the i386 and x86/64 instruction "lidt", 
    	which tells the CPU where the IDT is located and that it should be loaded.
    
    NOTES:
    	The internal implementation is located in "load_idt.asm".
    
        This header is written in C rather than C++
        to ensure maximum compatibility, 
        as Assembly provides a C interface, but not a C++ interface.
*/

#ifndef LOAD_IDT_H
#define LOAD_IDT_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void load_idt(uintptr_t);

#ifdef __cplusplus
}
#endif

#endif // LOAD_IDT_H
