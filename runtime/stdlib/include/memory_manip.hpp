/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    Under the MIT License
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This library defines basic routines for memory manipulation
    that can be used sys-wide.

    The routines include copying, moving, comparing, and filling memory

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
    class Memory_Manipulation final {
    public:
        static inline 
        status_t copy_memory_block(_INOUT_ void* dest_ptr, 
                                   _IN_    const void* src_ptr, 
                                   _IN_    uint32_t size) noexcept {
            status_t status;
            uint8_t* dest;
            const uint8_t* src;

            if (!dest_ptr || !src_ptr) [[unlikely]] {
                status = status::NULL_POINTER;
                goto cleanup;
            }

            if (size == 0) [[unlikely]] {
                status = status::SUCCESS;
                goto cleanup;
            }

            dest = static_cast<uint8_t*>(dest_ptr);
            src  = static_cast<const uint8_t*>(src_ptr);
            while (size--) {
                *dest++ = *src++;
            }

            status = status::SUCCESS;

        cleanup:
            return status;
        }

        static inline 
        status_t move_memory_block(_INOUT_ void* dest_ptr, 
                                   _IN_    const void* src_ptr, 
                                   _IN_    uint32_t size) noexcept {
            status_t status;
            uint8_t* dest;
            const uint8_t* src;

            if (!dest_ptr || !src_ptr) [[unlikely]] {
                status = status::NULL_POINTER;
                goto cleanup;
            }

            if (size == 0) [[unlikely]] {
                status = status::SUCCESS;
                goto cleanup;
            }
            
            dest = static_cast<uint8_t*>(dest_ptr);
            src  = static_cast<const uint8_t*>(src_ptr);

            if (reinterpret_cast<uint32_t>(dest) < 
                reinterpret_cast<uint32_t>(src)) [[likely]] {
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

            status = status::SUCCESS;

        cleanup:
            return status;
        }

        static inline 
        status_t set_memory_block(_INOUT_ void* dest_ptr, 
                                  _IN_    const int32_t value, 
                                  _IN_    uint32_t size) noexcept {
            status_t status;
            uint8_t* dest;
            uint8_t byte;

            if (!dest_ptr) [[unlikely]] {
                status = status::NULL_POINTER;
                goto cleanup;
            }

            if (size == 0) [[unlikely]] {
                status = status::SUCCESS;
                goto cleanup;
            }

            dest = static_cast<uint8_t*>(dest_ptr);
            byte = static_cast<uint8_t>(value);
            while (size--)
                *dest++ = byte;

            status = status::SUCCESS;

        cleanup:
            return status;
        };

        static inline 
        status_t compare_memory_block(_IN_ const void* a_ptr, 
                                      _IN_ const void* b_ptr,
                                      _IN_ uint32_t size) noexcept {
            status_t status;
            const uint8_t* a = static_cast<const uint8_t*>(a_ptr);
            const uint8_t* b = static_cast<const uint8_t*>(b_ptr);

            if (!a_ptr || !b_ptr) [[unlikely]] {
                status = status::NULL_POINTER;
                goto cleanup;
            }

            if (size == 0) [[unlikely]] {
                status = status::EQUAL_TO;
                goto cleanup;
            }

            while (size--) {
                if (*a < *b) {
                    status = status::LESS_THAN;
                    goto cleanup;  
                }

                if (*a > *b) {
                    status = status::GREATER_THAN;
                    goto cleanup;  
                }

                a++;
                b++;
            }

            status = status::EQUAL_TO;

        cleanup:
            return status;
        }

        Memory_Manipulation() noexcept  = default;
        ~Memory_Manipulation() noexcept = default;
    };
} // namespace stdlib
