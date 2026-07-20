/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This class defines the entire sys heap using a block allocator.
    The allocator uses linker-defined symbols 
    ("heap_start" and "heap_end") to determine the valid heap area and 
    moves a single pointer with each allocation by the amount x.

NOTES:
*/

#include "heap/block.hpp"

namespace kernel::heap
{
    void Block_Allocator::init(const uint8_t* begin, const uint8_t* end) noexcept {
        const uint32_t start_addr = reinterpret_cast<uint32_t>(begin);
        const uint32_t end_addr   = reinterpret_cast<uint32_t>(end);

        if (end_addr <= start_addr) [[unlikely]]
            sys::panic("Invalid memory range for heap initialization");

        const uint32_t total_raw_bytes = static_cast<uint32_t>(end_addr - start_addr);
        total_memory_blocks            = total_raw_bytes / (MEMORY_BLOCK_BYTE_SIZE +
                                         sizeof(uint16_t) + sizeof(uint8_t));
        if (total_memory_blocks <= 0) [[unlikely]]
            sys::panic("Heap area too small");

        allocation_sizes   = reinterpret_cast<uint16_t*>(start_addr);
        free_memory_blocks = reinterpret_cast<bool*>(start_addr + 
                                                     (total_memory_blocks *
                                                      sizeof(uint16_t)));

        int32_t data_pool_start = start_addr +
                                  (total_memory_blocks * sizeof(uint16_t)) +
                                  (total_memory_blocks * sizeof(bool));

        data_pool_start = (data_pool_start + (MEMORY_BLOCK_BYTE_SIZE - 1)) &
                          ~(MEMORY_BLOCK_BYTE_SIZE - 1);

        required_memory_pool_space = total_memory_blocks * MEMORY_BLOCK_BYTE_SIZE;
        if (data_pool_start + required_memory_pool_space > end_addr) [[unlikely]] {
            total_memory_blocks--;
            required_memory_pool_space = total_memory_blocks * MEMORY_BLOCK_BYTE_SIZE;
        }

        memory_pool_ptr = reinterpret_cast<uint8_t*>(data_pool_start);

        const uint32_t null = 0;
        const bool _true    = true;
        for (uint32_t i = 0; i < total_memory_blocks; ++i) {
            allocation_sizes[i]   = null;
            free_memory_blocks[i] = _true;
        }
    }

    void* Block_Allocator::set_allocation_sizes_entry(const uint32_t blocks_needed,
                                                      const uint32_t i) noexcept {
        const bool _false = false;
        for (uint32_t k = 0; k < blocks_needed; ++k)
            free_memory_blocks[i + k] = _false;

        allocation_sizes[i] = static_cast<uint16_t>(blocks_needed);
        return &memory_pool_ptr[i * MEMORY_BLOCK_BYTE_SIZE];
    }

    bool Block_Allocator::fiend_enough_free_memory_blocks(const uint32_t i, 
                                                          const uint32_t blocks_needed,
                                                          uint32_t* j) noexcept {
        *j = 0;
        while (*j < blocks_needed) {
            if (!free_memory_blocks[i + *j])
                return false;

            (*j)++;
        }

        return true;
    }

    [[nodiscard]]
    void* Block_Allocator::allocate(const uint32_t allocated_bytes) noexcept {
        if (memory_pool_ptr == nullptr) [[unlikely]]
            sys::panic("Block allocator not initialized");

        if (allocated_bytes == 0) [[unlikely]]
            return nullptr;

        const uint32_t blocks_needed = (allocated_bytes + MEMORY_BLOCK_BYTE_SIZE - 1) / 
                                       MEMORY_BLOCK_BYTE_SIZE;
        
        if (blocks_needed > total_memory_blocks) [[unlikely]]
            return nullptr;

        uint32_t i = 0;
        uint32_t j = 0;
        while (i <= total_memory_blocks - blocks_needed) {
            if (fiend_enough_free_memory_blocks(i, blocks_needed, &j))
                return set_allocation_sizes_entry(blocks_needed, i);
            else
                i += j + 1;
        }

        return nullptr;
    }

    void Block_Allocator::deallocate(void* ptr) noexcept {
        if ((ptr == nullptr) || (memory_pool_ptr == nullptr)) [[unlikely]]
            return;

        const uint8_t* block_ptr = reinterpret_cast<uint8_t*>(ptr);
        if (block_ptr < memory_pool_ptr || 
            block_ptr >= memory_pool_ptr + required_memory_pool_space) [[unlikely]]
            sys::panic("Attempted to free pointer outside heap");

        const uint32_t memory_offset = static_cast<uint32_t>(block_ptr - memory_pool_ptr);
        if (memory_offset % MEMORY_BLOCK_BYTE_SIZE != 0) [[unlikely]]
            sys::panic("Attempted to free misaligned pointer");

        const uint32_t memory_pool_start_index = memory_offset / MEMORY_BLOCK_BYTE_SIZE;
        const uint32_t memory_blocks_to_free = allocation_sizes[memory_pool_start_index];

        if (memory_blocks_to_free == 0) [[unlikely]]
            sys::panic("Double free or freeing memory that was not allocated");

        const bool _true = true;
        for (uint32_t i = 0; i < memory_blocks_to_free; ++i)
            free_memory_blocks[memory_pool_start_index + i] = _true;

        allocation_sizes[memory_pool_start_index] = 0;
    }
} // namespace kernel::heap
