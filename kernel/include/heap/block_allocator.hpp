/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This class defines the entire system heap using a block allocator.
    The allocator uses linker-defined symbols 
    ("heap_start" and "heap_end") to determine the valid heap area and 
    moves a single pointer with each allocation by the amount x.

NOTES:
*/

#pragma once

#include "utils/heap_pos_marker.hpp"
#include <stdint.h>
#include <array.hpp>
#include <kernel_arch_api.hpp>
#include <text_output.hpp>

namespace kernel::heap
{
    class Block_Allocator final {
    private:
        static constexpr uint32_t MEMORY_BLOCK_BYTE_SIZE = 8;

        uint32_t total_memory_blocks = 0;
        uint32_t required_memory_pool_space = 0;

        uint8_t* memory_pool_ptr = nullptr;
        uint16_t* allocation_sizes;
        bool* free_memory_blocks;

        void* set_allocation_sizes_entry(const uint32_t blocks_neede, 
                                         const uint32_t i) 
                                         noexcept;

        bool fiend_enough_free_memory_blocks(const uint32_t i, 
                                             const uint32_t blocks_needed,
                                             uint32_t* j) 
                                             noexcept;

    public:
        void init(const uint8_t* begin, const uint8_t* end) noexcept;
        
        [[nodiscard]]
        void* allocate(const uint32_t allocated_bytes) noexcept;
        void deallocate(void* ptr) noexcept;

        Block_Allocator() noexcept = default;
        ~Block_Allocator() noexcept = default;
    };

    extern Block_Allocator block;
} // namespace kernel::heap
