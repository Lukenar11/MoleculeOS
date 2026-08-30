/**
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
        stdlib::Memory_Manipulation::copy_memory_block(dest_ptr, 
                                                       src_ptr,
                                                       size);
                                                    
        return dest_ptr;
    }

    void* memmove(void* dest_ptr, const void* src_ptr, uint32_t size) {
        stdlib::Memory_Manipulation::move_memory_block(dest_ptr, 
                                                       src_ptr,
                                                       size);

        return dest_ptr;
    }

    void* memset(void* dest_ptr, int32_t value, uint32_t size) {
        stdlib::Memory_Manipulation::set_memory_block(dest_ptr, 
                                                      value,
                                                      size);

        return dest_ptr;
    }

    int32_t memcmp(const void* a_ptr, const void* b_ptr, uint32_t size) {
        int32_t iso_std_status;
        status_t status;

        status = stdlib::Memory_Manipulation::compare_memory_block(a_ptr,
                                                                   b_ptr, 
                                                                   size);
        switch (status & status::flags::SIZE_ZERO) {
        case status::LESS_THAN: 
            iso_std_status = -1;
            break;

        case status::GREATER_THAN: 
            iso_std_status = 1;
            break;

        default: 
            iso_std_status = 0;
            break;
        }

        return iso_std_status;
    }

    void strncpy(char* dest_ptr, const char* src_ptr, uint32_t size) {
        stdlib::String_Manipulation::copy_string_part(dest_ptr, 
                                                      src_ptr, 
                                                      size);
    }

    void strcpy(char* dest_ptr, const char* src_ptr) {
        stdlib::String_Manipulation::copy_string(dest_ptr, 
                                                 src_ptr);
    }

    uint32_t strlen(const char *string) {
        uint32_t length;
        stdlib::String_Manipulation::get_string_length(length, string);

        return length;
    }

    int32_t strcmp(const char* a_ptr, const char* b_ptr) {
        int32_t status;

        status = stdlib::String_Manipulation::compare_strings(a_ptr, b_ptr);

        switch (status) {
        case status::LESS_THAN: 
            status = -1;
            break;

        case status::GREATER_THAN: 
            status = 1;
            break;

        default: 
            status = 0;
            break;
        }

        return status;
    }

    const char* strchr(const char* string, int32_t symbol) {
        const char* result = nullptr; 
        if (stdlib::String_Manipulation::find_char_in_string(result,
                                                             string,
                                                             symbol) != 
            status::SUCCESS) [[unlikely]] {
            return nullptr; 
        }

        return result;
    }
}
