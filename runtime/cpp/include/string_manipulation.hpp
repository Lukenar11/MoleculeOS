#pragma once

#include <stdint.h>

namespace runtime
{
    void copy_string(char* dest_ptr, const char* src_ptr) noexcept;
    
    void copy_n_string_elements(char* dest_ptr,
                                const char* src_ptr, 
                                uint32_t size) noexcept;

    const char* includes_char_at_pos(const char* string, 
                                     int32_t symbol) noexcept;

    uint32_t string_length(const char *string) noexcept;
    int32_t compare_strings(const char* a_ptr, const char* b_ptr) noexcept;
} // namespace runtime
