/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This library contains a C-like runtime for basic string and
    memory manipulation.

    The actual implementation of the runtime is hidden in "memory_manip" and
    "string_manip"; this is merely a C interface.

NOTES:
    This header is purely an ABI compatibility layer; 
    it is included only because all 
    C and C++ compilers require a basic C runtime and 
    do not accept custom implementations with names or
    signatures different from those described in the C standard.

    The wrappers for these functions are located in the 
    files "compiler_c_api.hpp" and "compiler_c_api.cpp".
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
int32_t memcmp(const void* a_ptr, const void* b_ptr, uint32_t size);
void strncpy(char* dest_ptr, const char* src_ptr, uint32_t size);
void strcpy(char* dest_ptr, const char* src_ptr);
uint32_t strlen(const char *string);
int32_t strcmp(const char* a_ptr, const char* b_ptr);
const char* strchr(const char* string, int32_t symbol);

#ifdef __cplusplus
}
#endif

#endif // STRING_H
