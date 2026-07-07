/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    Under the MIT License
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This library defines basic routines for memory manipulation
    that can be used system-wide.

    The routines include copying, moving, comparing, and filling memory

NOTES:
    Because of the C API for the compiler, the methods are defined in the
    header so that the compiler can inline them.
*/

#pragma once

#include <stdint.h>

namespace runtime
{
    class Memory_Manipulation final {
    public:
        static inline void* copy_memory_block(void* dest_ptr, 
                                              const void* src_ptr, 
                                              uint32_t size) noexcept {
            uint8_t* dest = static_cast<uint8_t*>(dest_ptr);
            const uint8_t* src = static_cast<const uint8_t*>(src_ptr);
            while (size--)
                *dest++ = *src++;

            return dest_ptr;
        }

        static inline void* move_memory_block(void* dest_ptr, 
                                              const void* src_ptr, 
                                              uint32_t size) noexcept {
            uint8_t* dest = static_cast<uint8_t*>(dest_ptr);
            const uint8_t* src = static_cast<const uint8_t*>(src_ptr);

            if (reinterpret_cast<uint32_t>(dest) < 
                reinterpret_cast<uint32_t>(src)) {
                while (size--)
                    *dest++ = *src++;
            } else {
                dest += size;
                src += size;
                while (size--)
                    *--dest = *--src;
            }

            return dest_ptr;
        }

        static inline void* set_memory_block(void* dest_ptr, 
                                             int32_t value, 
                                             uint32_t size) noexcept {
            uint8_t* dest = static_cast<uint8_t*>(dest_ptr);
            uint8_t byte = static_cast<uint8_t>(value);
            while (size--)
                *dest++ = byte;

            return dest_ptr;
        };

        static inline int32_t compare_memory_block(const void* a_ptr, 
                                                   const void* b_ptr,
                                                   uint32_t size) noexcept {
            const uint8_t* a = static_cast<const uint8_t*>(a_ptr);
            const uint8_t* b = static_cast<const uint8_t*>(b_ptr);
            while (size--) {
                if (*a != *b)
                    return static_cast<int32_t>(*a) - static_cast<int32_t>(*b);

                a++;
                b++;
            }

            return 0;
        }

        Memory_Manipulation() noexcept = default;
        ~Memory_Manipulation() noexcept = default;
    };
} // namespace runtime
