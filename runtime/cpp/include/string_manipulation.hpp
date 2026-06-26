/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    Under the MIT License
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This library defines basic routines for string manipulation
    that can be used system-wide.

    The routines include copying, shifting, comparing, and filling strings

NOTES:
    Because of the C API for the compiler, the methods are defined in the
    header so that the compiler can inline them.
*/

#pragma once

#include <stdint.h>

namespace runtime
{
    class String_Manipulation final {
    public:
        inline void copy_string(char* dest_ptr, const char* src_ptr) noexcept {
            while ((*dest_ptr++ = *src_ptr++));
        }

        inline void copy_string_part(char* dest_ptr, 
                                     const char* src_ptr, 
                                     uint32_t size) noexcept {
            const char null_char = '\0';

            for (uint32_t i = 0; i < size; i++) {
                if (src_ptr[i] == null_char) {
                    dest_ptr[i] = null_char;
                    return;
                }

                dest_ptr[i] = src_ptr[i];
            }

            dest_ptr[size - 1] = null_char;
        }

        inline const char* find_char_in_string(const char* string, 
                                               int32_t symbol) noexcept {
            while (*string != static_cast<char>(symbol))
                if (!(*string++)) [[unlikely]]
                    return nullptr;

            return const_cast<char*>(string);
        }

        inline uint32_t get_string_length(const char *string) noexcept {
            uint32_t length = 0;
            while (*string != '\0') {
                string++;
                length++;
            }

            return length;
        }

        inline int32_t compare_strings(const char* a_ptr, const char* b_ptr) 
                                       noexcept {
            while (*a_ptr && (*a_ptr == *b_ptr)) {
                ++a_ptr;
                ++b_ptr;
            }

            return static_cast<uint8_t>(*a_ptr) - static_cast<uint8_t>(*b_ptr);
        }

        String_Manipulation() noexcept = default;
        ~String_Manipulation() noexcept = default;
    };

    extern String_Manipulation string_manip;
} // namespace runtime
