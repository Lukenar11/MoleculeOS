/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This library declares the basic C-like memory and string
    manipulation routines that can be used throughout the system.

    The routines include memory copy, move, compare, fill
    and basic string comparisons, all manually implemented to
    ensure deterministic behavior in a freestanding kernel environment.

NOTES:
    This header is written in C to guarantee compatibility with both
    C and C++ kernel components, particularly
    those that interact with low-level memory and buffer utilities.
*/

#ifndef STRING_H
#define STRING_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void* memcpy(void* dest_ptr, const void* src_ptr, uint32_t size);
void* memmove(void* dest_ptr, const void* src_ptr, uint32_t size);
void* memset(void* dest_ptr, const int32_t value, uint32_t size);
void strncpy(char* dest_ptr, const char* src_ptr, uint32_t size);
void strcpy(char* dest_ptr, const char* src_ptr);
uint32_t strlen(char *string);
int32_t memcmp(const void* a_ptr, const void* b_ptr, uint32_t size);
int32_t strcmp(const char* a_ptr, const char* b_ptr);

#ifdef __cplusplus
}
#endif

#endif
