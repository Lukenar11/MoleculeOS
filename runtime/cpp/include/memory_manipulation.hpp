#pragma once

#include <stdint.h>

namespace runtime
{
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
} // namespace runtime
