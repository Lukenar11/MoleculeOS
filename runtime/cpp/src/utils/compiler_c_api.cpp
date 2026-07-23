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

#include "utils/compiler_c_api.hpp"

extern "C"
{
    void* memcpy(void* dest_ptr, const void* src_ptr, uint32_t size) {
        return runtime::Memory_Manipulation::copy_memory_block(dest_ptr, 
                                                               src_ptr,
                                                               size);
    }

    void* memmove(void* dest_ptr, const void* src_ptr, uint32_t size) {
        return runtime::Memory_Manipulation::move_memory_block(dest_ptr, 
                                                               src_ptr,
                                                               size);
    }

    void* memset(void* dest_ptr, int32_t value, uint32_t size) {
        return runtime::Memory_Manipulation::set_memory_block(dest_ptr, 
                                                              value,
                                                              size);
    }

    int32_t memcmp(const void* a_ptr, const void* b_ptr, uint32_t size) {
        return runtime::Memory_Manipulation::compare_memory_block(a_ptr,
                                                                  b_ptr, 
                                                                  size);
    }

    void strncpy(char* dest_ptr, const char* src_ptr, uint32_t size) {
        runtime::String_Manipulation::copy_string_part(dest_ptr, 
                                                       src_ptr, 
                                                       size);
    }

    void strcpy(char* dest_ptr, const char* src_ptr) {
        runtime::String_Manipulation::copy_string(dest_ptr, 
                                                  src_ptr);
    }

    uint32_t strlen(const char *string) {
        return runtime::String_Manipulation::get_string_length(string);
    }

    int32_t strcmp(const char* a_ptr, const char* b_ptr) {
        return runtime::String_Manipulation::compare_strings(a_ptr, 
                                                             b_ptr);
    }

    const char* strchr(const char* string, int32_t symbol) {
        return runtime::String_Manipulation::find_char_in_string(string, 
                                                                 symbol);
    }
}
