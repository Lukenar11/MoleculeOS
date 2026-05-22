/*
    LICENSE:
        Copyright (c) 2026 Lukenar11 (Luke Matthes)
        MIT Licensed
        https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
    
    DESCRIPTION:
    	This file contains 2 simple assembly routines that work using stack operations.

        The function "save_eflags" saves the current CPU flags (flag register).
        The function "restore_eflags" restores the CPU flags (flag register).
    
    NOTES:
    	The internal implementation is located in "cpu_flags.asm".
    
        This header is written in C rather than C++
        to ensure maximum compatibility, 
        as Assembly provides a C interface, but not a C++ interface.
*/

#ifndef CPU_FLAGS
#define CPU_FLAGS

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

uint32_t save_eflags(void);
void restore_eflags(uint32_t);

#ifdef __cplusplus
}
#endif

#endif // CPU_FLAGS
