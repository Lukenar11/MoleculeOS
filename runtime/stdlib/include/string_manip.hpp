/**
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

#include <sal.hpp>
#include <status.hpp>
#include <types.hpp>

namespace stdlib
{
    class String_Manipulation final {
    public:
        static inline 
        status_t copy_string_part(_INOUT_ char* dest_ptr,
                                  _IN_    const char* src_ptr,
                                  _IN_    const uint32_t size) noexcept {
            status_t status;
            const char null_char = '\0';

            if (!dest_ptr || !src_ptr) [[unlikely]] {
                status = status::NULL_POINTER;
                goto cleanup;
            }

            if (size == 0) [[unlikely]] {
                status = status::SUCCESS;
                goto cleanup;
            }

            for (uint32_t i = 0; i < size; i++) [[likely]] {
                if (src_ptr[i] == null_char) {
                    dest_ptr[i] = null_char;
                    status = status::SUCCESS;
                    goto cleanup;
                }

                dest_ptr[i] = src_ptr[i];
            }

            dest_ptr[size - 1] = null_char;
            status = status::SUCCESS;

        cleanup:
            return status;
        }

        static inline 
        status_t copy_string(_INOUT_ char* dest_ptr, 
                             _IN_    const char* src_ptr) noexcept {
            status_t status;

            if (!dest_ptr || !src_ptr) [[unlikely]] {
                status = status::NULL_POINTER;
                goto cleanup;
            }

            while ((*dest_ptr++ = *src_ptr++)) [[likely]] {
            }

            status = status::SUCCESS;

        cleanup:
            return status;
        }

        static inline 
        status_t find_char_in_string(_OUT_ const char*& founded_char,
                                     _IN_  const char* string, 
                                     _IN_  const int32_t symbol) noexcept {
            status_t status;

            if (!string) [[unlikely]] {
                founded_char = nullptr;
                status       = status::NULL_POINTER;

                goto cleanup;
            }

            while (*string != static_cast<char>(symbol)) [[likely]] {
                if (!(*string++)) [[unlikely]] {
                    founded_char = nullptr;
                    status       = status::NOT_FOUND;

                    goto cleanup;
                }
            }

            founded_char = const_cast<char*>(string);
            status       = status::SUCCESS;

        cleanup:
            return status;
        }

        static inline 
        status_t get_string_length(_OUT_ uint32_t& length,
                                   _IN_  const char *string) noexcept {
            status_t status;
            length = 0;

            if (!string) [[unlikely]] {
                status = status::NULL_POINTER;
                goto cleanup;
            }

            while (*string != '\0') [[likely]] {
                ++string;
                ++length;
            }

            status = status::SUCCESS;

        cleanup:
            return status;
        }

        static inline 
        status_t compare_strings(_IN_ const char* a_ptr, 
                                 _IN_ const char* b_ptr) noexcept {
            status_t status;

            if (!a_ptr || !b_ptr) [[unlikely]] {
                status = status::NULL_POINTER;
                goto cleanup;
            }

            while (*a_ptr && (*a_ptr == *b_ptr)) [[likely]] {
                ++a_ptr;
                ++b_ptr;
            }

            if (*a_ptr < *b_ptr) {
                status = status::LESS_THAN;
            } 
            else if (*a_ptr > *b_ptr) {
                status = status::GREATER_THAN;
            }
            else {
                status = status::EQUAL_TO;
            }

        cleanup:
            return status;
        }

        static inline 
        status_t string_to_int(_OUT_ int32_t& value,
                               _IN_  const char* txt) noexcept {
            status_t status;
            value = 0;

            if (!txt) [[unlikely]] {
                status = status::NULL_POINTER;
                goto cleanup;
            }

            while (*txt) [[likely]] {
                if (!is_digit(*txt)) [[unlikely]] {
                    status = status::INVALID_PARAMETER;
                    goto cleanup;
                }

                value = value * 10 + (*txt - '0');
                ++txt;
            }

            status = status::SUCCESS;

        cleanup:
            return status;
        }

        static inline 
        bool is_digit(_IN_ const int32_t symbol) noexcept {
            return (symbol >= '0') && (symbol <= '9');
        }

        String_Manipulation() noexcept  = default;
        ~String_Manipulation() noexcept = default;
    };
} // namespace stdlib
