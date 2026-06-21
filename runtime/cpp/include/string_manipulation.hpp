#pragma once

#include <stdint.h>

namespace runtime
{
    class String_Manipulation final {
    public:
        void copy_string(char* dest_ptr, const char* src_ptr) noexcept;
        
        void copy_string_part(char* dest_ptr,
                              const char* src_ptr, 
                              uint32_t size) noexcept;

        const char* find_char_in_string(const char* string, 
                                        int32_t symbol) noexcept;

        uint32_t get_string_length(const char *string) noexcept;
        int32_t compare_strings(const char* a_ptr, const char* b_ptr) noexcept;

        String_Manipulation() noexcept = default;
        ~String_Manipulation() noexcept = default;
    };

    extern String_Manipulation string_manip;
} // namespace runtime
