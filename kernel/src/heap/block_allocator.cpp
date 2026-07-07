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

#include "heap/block_allocator.hpp"

namespace kernel::heap
{
    void Block_Allocator::init(const uint8_t* begin, const uint8_t* end) noexcept {
        const uint32_t memory_pool_start = reinterpret_cast<uint32_t>(begin);
        const uint32_t memory_pool_end   = reinterpret_cast<uint32_t>(end);
        const uint32_t memory_pool_size  = memory_pool_end - memory_pool_start;

        total_memory_blocks        = memory_pool_size / MEMORY_BLOCK_BYTE_SIZE;
        required_memory_pool_space = total_memory_blocks * MEMORY_BLOCK_BYTE_SIZE;

        allocation_sizes   = reinterpret_cast<uint16_t*>(memory_pool_start);
        free_memory_blocks = reinterpret_cast<bool*>(memory_pool_start + 
                                                     total_memory_blocks * 
                                                     sizeof(uint16_t));

        const uint32_t aligned_start = (memory_pool_start + (MEMORY_BLOCK_BYTE_SIZE - 1))
                                       & ~(MEMORY_BLOCK_BYTE_SIZE - 1);

        if ((aligned_start < memory_pool_end) &&
            (memory_pool_end - aligned_start) >= required_memory_pool_space)
            memory_pool_ptr = reinterpret_cast<uint8_t*>(aligned_start);
        else
            memory_pool_ptr = nullptr;
    }

    void* Block_Allocator::set_allocation_sizes_entry(const uint32_t blocks_needed,
                                                      const uint32_t i) 
                                                      noexcept {
        for (uint32_t k = 0; k < blocks_needed; ++k)
            free_memory_blocks[i + k] = false;
                
        allocation_sizes[i] = static_cast<uint16_t>(blocks_needed);
        return &memory_pool_ptr[i * MEMORY_BLOCK_BYTE_SIZE];
    }

    bool Block_Allocator::fiend_enough_free_memory_blocks(const uint32_t i, 
                                                          const uint32_t blocks_needed,
                                                          uint32_t* j) 
                                                          noexcept {
        bool enough_free_memory_blocks_found = true;
        *j = 0;

        while (*j < blocks_needed) {
            if (!free_memory_blocks[i + *j]) {
                enough_free_memory_blocks_found = false;
                break;
            }
            (*j)++;
        }

        return enough_free_memory_blocks_found;
    }

    [[nodiscard]]
    void* Block_Allocator::allocate(const uint32_t allocated_bytes) noexcept {
        if (memory_pool_ptr == nullptr) [[unlikely]]
            system::panic("Block allocator not initialized");

        if (allocated_bytes == 0) [[unlikely]]
            return nullptr;

        const uint32_t blocks_needed = (allocated_bytes + MEMORY_BLOCK_BYTE_SIZE - 1) / 
                                        MEMORY_BLOCK_BYTE_SIZE;
        if (blocks_needed > total_memory_blocks) [[unlikely]]
            system::panic("Requested allocation exceeds heap capacity");

        uint32_t i = 0;
        uint32_t j = 0;
        while (i <= total_memory_blocks - blocks_needed) {
            if (fiend_enough_free_memory_blocks(i, blocks_needed, &j)) {
                return set_allocation_sizes_entry(blocks_needed, i);
            } else {
                i += j + 1;
            }
        }
        
        return nullptr;
    }
    
    void Block_Allocator::deallocate(void* ptr) noexcept {
        if ((ptr == nullptr) || (memory_pool_ptr == nullptr)) [[unlikely]]
            return;
            
        const uint8_t* block_ptr = reinterpret_cast<uint8_t*>(ptr);
        if (block_ptr < memory_pool_ptr ||
            block_ptr >= memory_pool_ptr + required_memory_pool_space) [[unlikely]]
            system::panic("Attempted to free pointer outside heap");

        const uint32_t memory_offset           = static_cast<uint32_t>(block_ptr - 
                                                                       memory_pool_ptr);
        const uint32_t memory_pool_start_index = memory_offset / MEMORY_BLOCK_BYTE_SIZE;
        if (memory_pool_start_index >= total_memory_blocks) [[unlikely]]
            system::panic("Invalid block index during deallocation");

        const uint32_t memory_blocks_to_free = allocation_sizes[memory_pool_start_index];
        if (memory_blocks_to_free == 0) [[unlikely]]
            return;

        for (uint32_t i = 0; i < memory_blocks_to_free; ++i)
            if (memory_pool_start_index + i < total_memory_blocks)
                free_memory_blocks[memory_pool_start_index + i] = true;
    }
} //namespace kernel::heap
