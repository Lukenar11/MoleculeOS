#pragma once

#include <stdint.h>

namespace runtime
{
    class Memory_Manipulation final {
    public:
        void* copy_memory_block(void* dest_ptr,
                                const void* src_ptr, 
                                uint32_t size) noexcept;

        void* move_memory_block(void* dest_ptr,
                                const void* src_ptr, 
                                uint32_t size) noexcept;

        void* set_memory_block(void* dest_ptr, 
                               const int32_t value, 
                               uint32_t size) noexcept;

        int32_t compare_memory_block(const void* a_ptr,
                                     const void* b_ptr,
                                     uint32_t size) noexcept;

        Memory_Manipulation() noexcept = default;
        ~Memory_Manipulation() noexcept = default;
    };

    extern Memory_Manipulation memory_manip;
} // namespace runtime
