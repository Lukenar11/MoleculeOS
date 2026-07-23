/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This library contains wrappers for a C-like runtime environment for
    basic string and memory manipulation.

NOTES:
    This header must not be used in code; it is included only because all 
    C and C++ compilers require a basic 'C runtime' environment and 
    do not accept user-defined implementations whose names or
    signatures differ from those described in the 'C standard'.
*/

#include <memory_manip.hpp>
#include <string_manip.hpp>

extern "C"
{
    void* memcpy(void* dest_ptr, const void* src_ptr, uint32_t size);
    void* memmove(void* dest_ptr, const void* src_ptr, uint32_t size);
    void* memset(void* dest_ptr, int32_t value, uint32_t size);
    int32_t memcmp(const void* a_ptr, const void* b_ptr, uint32_t size);
    void strncpy(char* dest_ptr, const char* src_ptr, uint32_t size);
    void strcpy(char* dest_ptr, const char* src_ptr);
    uint32_t strlen(const char *string);
    int32_t strcmp(const char* a_ptr, const char* b_ptr);
    const char* strchr(const char* string, int32_t symbol);
}
