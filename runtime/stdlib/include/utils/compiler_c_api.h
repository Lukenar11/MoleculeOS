/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This library contains a small c runtime for the compiler.

NOTES:
    Ths Code is only for the compiler and not for the usage in this project.
*/


#ifndef COMPILER_C_API
#define COMPILER_C_API

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C"
{
#endif

void* memcpy(void* dest_ptr, const void* src_ptr, uint32_t size);
void* memmove(void* dest_ptr, const void* src_ptr, uint32_t size);
void* memset(void* dest_ptr, int32_t value, uint32_t size);
int32_t memcmp(const void* a, const void* b, uint32_t size);

void strcpy(char* dest_ptr, const char* src_ptr);
void strncpy(char* dest_ptr, const char* src_ptr, uint32_t size);
uint32_t strlen(const char* string);
int32_t strcmp(const char* a, const char* b);
const char* strchr(const char* string, int32_t symbol);

#ifdef __cplusplus
}
#endif

#endif // COMPILER_C_API
