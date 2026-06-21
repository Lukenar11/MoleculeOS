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

// #include "string.h"
// 
// void* memcpy(void* dest_ptr, const void* src_ptr, uint32_t size)
// {
//     uint8_t* dest = (uint8_t*)dest_ptr;
//     const uint8_t* src = (const uint8_t*)src_ptr;
//     while (size--)
//         *dest++ = *src++;
// 
//     return dest_ptr;
// }
// 
// void* memmove(void* dest_ptr, const void* src_ptr, uint32_t size)
// {
//     uint8_t* dest = (uint8_t*)dest_ptr;
//     const uint8_t* src = (const uint8_t*)src_ptr;
//     if ((uintptr_t)dest < (uintptr_t)src) {
//         while (size--)
//             *dest++ = *src++;
//     } else {
//         dest += size;
//         src += size;
//         while (size--)
//             *--dest = *--src;
//     }
// 
//     return dest_ptr;
// }
// 
// void* memset(void* dest_ptr, int32_t value, uint32_t size)
// {
//     uint8_t* dest = (uint8_t*)dest_ptr;
//     uint8_t byte = (uint8_t)value;
//     while (size--)
//         *dest++ = byte;
// 
//     return dest_ptr;
// }
// 
// void strncpy(char* dest_ptr, const char* src_ptr, uint32_t size)
// {
//     const char null_char = '\0';
// 
//     for (uint32_t i = 0; i < size; i++) {
//         if (src_ptr[i] == null_char) {
//             dest_ptr[i] = null_char;
//             return;
//         }
// 
//         dest_ptr[i] = src_ptr[i];
//     }
// 
//     dest_ptr[size - 1] = null_char;
// }
// 
// void strcpy(char* dest_ptr, const char* src_ptr)
// {
//     while ((*dest_ptr++ = *src_ptr++));
// }
// 
// uint32_t strlen(const char *string) 
// {
//     uint32_t length = 0;
//     while (*string != '\0') {
//         string++;
//         length++;
//     }
//     
//     return length;
// }
// 
// int32_t memcmp(const void* a_ptr, const void* b_ptr, uint32_t size)
// {
//     const uint8_t* a = (const uint8_t*)a_ptr;
//     const uint8_t* b = (const uint8_t*)b_ptr;
//     while (size--) {
//         if (*a != *b)
//             return (int32_t)*a - (int32_t)*b;
//         a++;
//         b++;
//     }
// 
//     return 0;
// }
// 
// int32_t strcmp(const char* a_ptr, const char* b_ptr)
// {
//     while (*a_ptr && (*a_ptr == *b_ptr)) {
//         ++a_ptr;
//         ++b_ptr;
//     }
// 
//     return (uint8_t)*a_ptr - (uint8_t)*b_ptr;
// }
// 
// const char* strchr(const char* string, int32_t symbol) 
// {
//     while (*string != (char)symbol)
//         if (!*string++)
//             return (char*)0;
// 
//     return (char*)string;
// }
// 