#include "memory_manipulation.hpp"

namespace runtime
{
    void* copy_memory_block(void* dest_ptr, 
                            const void* src_ptr, 
                            uint32_t size) noexcept {
        uint8_t* dest = static_cast<uint8_t*>(dest_ptr);
        const uint8_t* src = static_cast<const uint8_t*>(src_ptr);
        while (size--)
            *dest++ = *src++;

        return dest_ptr;
    }

    void* move_memory_block(void* dest_ptr, 
                            const void* src_ptr, 
                            uint32_t size) noexcept {
        uint8_t* dest = static_cast<uint8_t*>(dest_ptr);
        const uint8_t* src = static_cast<const uint8_t*>(src_ptr);

        if (reinterpret_cast<uintptr_t>(dest) < 
            reinterpret_cast<uintptr_t>(src)) {
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

    void* set_memory_block(void* dest_ptr, 
                           int32_t value, 
                           uint32_t size) noexcept {
        uint8_t* dest = static_cast<uint8_t*>(dest_ptr);
        uint8_t byte = static_cast<uint8_t>(value);
        while (size--)
            *dest++ = byte;

        return dest_ptr;
    };

    int32_t compare_memory_block(const void* a_ptr, 
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
} // namespace runtime
