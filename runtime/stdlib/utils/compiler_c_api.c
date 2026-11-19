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


#include "utils/compiler_c_api.h"


void* 
memcpy(void* dest_ptr, const void* src_ptr, uint32_t size) 
{
    uint8_t* dest      = (uint8_t*)dest_ptr;
    const uint8_t* src = (const uint8_t*)src_ptr;

    while (size--) {
        *dest++ = *src++;
    }

    return dest_ptr;
}


void* 
memmove(void* dest_ptr, const void* src_ptr, uint32_t size) 
{
    uint8_t* dest      = (uint8_t*)dest_ptr;
    const uint8_t* src = (const uint8_t*)src_ptr;

    if ((uint32_t)dest < (uint32_t)src) {
        while (size--) {
            *dest++ = *src++;
        }
    } 
    else {
        dest += size;
        src  += size;

        while (size--) {
            *--dest = *--src;
        }
    }

    return dest_ptr;
}


void* 
memset(void* dest_ptr, const int32_t value, uint32_t size) 
{

    uint8_t* dest = (uint8_t*)dest_ptr;
    uint8_t byte  = (uint8_t)value;

    while (size--) {
        *dest++ = byte;
    }

    return dest_ptr;
}


int32_t 
memcmp(const void* a_ptr, const void* b_ptr, uint32_t size) 
{
    int32_t result;

    const uint8_t* a = (const uint8_t*)a_ptr;
    const uint8_t* b = (const uint8_t*)b_ptr;

    while (size--) {
        if (*a < *b) {
            result = -1;
            goto cleanup;  
        }

        if (*a > *b) {
            result = 1;
            goto cleanup;  
        }

        a++;
        b++;
        }

    result = 0;

cleanup:
    return result;
}


void 
strcpy(char* dest_ptr, const char* src_ptr) 
{
    while ((*dest_ptr++ = *src_ptr++)) {
    }
}


void 
strncpy(char* dest_ptr, const char* src_ptr, uint32_t size) 
{
    const char null_char = '\0';

    for (uint32_t i = 0; i < size; i++) {
        if (src_ptr[i] == null_char) {
            for (; i < size; i++) {
                dest_ptr[i] = '\0';
            }

            goto cleanup;
        }

        dest_ptr[i] = src_ptr[i];
    }

cleanup:
    return;
}


uint32_t 
strlen(const char* string) 
{
    uint32_t length = 0;

    while (*string != '\0') {
        ++string;
        ++length;
    }

    return length;
}


int32_t 
strcmp(const char* a_ptr, const char* b_ptr) 
{
    int32_t status;

    while (*a_ptr && (*a_ptr == *b_ptr)) {
        ++a_ptr;
        ++b_ptr;
    }

    if (*a_ptr < *b_ptr) {
        status = -1;
    } 
    else if (*a_ptr > *b_ptr) {
        status = 1;
    }
    else {
        status = 0;
    }

    return status;
}


const char* 
strchr(const char* string, int32_t symbol) 
{
    const char null_char = '\0';
    const char* result   = NULL;

    while (*string != (char)symbol) {
        if (*string == null_char) {
            goto cleanup;
        }

        string++;
    }

    result = string;

cleanup:
    return result;
}
