/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This class defines the entire system heap using a 'block allocator'.
    The allocator uses linker-defined symbols (heap_start & heap_end)
    to determine the valid heap area.

NOTES:
*/

#include "heap/block.hpp"

namespace kernel::heap
{
    void Block_Allocator::init(const uint8_t* begin, 
                               const uint8_t* end) noexcept {
        const uint32_t start_addr = reinterpret_cast<uint32_t>(begin);
        const uint32_t end_addr   = reinterpret_cast<uint32_t>(end);

        if (end_addr <= start_addr)
            sys::panic("Invalid memory range for heap initialization");

        const uint32_t total_raw_bytes = end_addr - start_addr;

        total_memory_blocks = total_raw_bytes / MEMORY_BLOCK_BYTE_SIZE;
        if (total_memory_blocks == 0)
            sys::panic("Heap area too small");

        const uint32_t bitmap_word_count = (total_memory_blocks + 31) / 32;

        allocation_sizes = reinterpret_cast<uint16_t*>(start_addr);

        free_memory_bitmap = reinterpret_cast<uint32_t*>(start_addr + 
                                                         total_memory_blocks * 
                                                         sizeof(uint16_t));

        uint32_t data_pool_start = start_addr +
                                   total_memory_blocks * sizeof(uint16_t) +
                                   bitmap_word_count * sizeof(uint32_t);

        data_pool_start = (data_pool_start + (MEMORY_BLOCK_BYTE_SIZE - 1)) &
                          ~(MEMORY_BLOCK_BYTE_SIZE - 1);

        required_memory_pool_space = total_memory_blocks * MEMORY_BLOCK_BYTE_SIZE;

        if (data_pool_start + required_memory_pool_space > end_addr) {
            total_memory_blocks--;
            required_memory_pool_space = total_memory_blocks * MEMORY_BLOCK_BYTE_SIZE;
        }

        memory_pool_ptr = reinterpret_cast<uint8_t*>(data_pool_start);

        const uint32_t set_all_free = 0xFFFF'FFFF;
        for (uint32_t i = 0; i < bitmap_word_count; ++i)
            free_memory_bitmap[i] = set_all_free;

        const uint32_t null = 0;
        for (uint32_t i = 0; i < total_memory_blocks; ++i)
            allocation_sizes[i] = null;
    }

    void* Block_Allocator::set_allocation_sizes_entry(const uint32_t blocks_needed, 
                                                      const uint32_t i) noexcept {
        for (uint32_t k = 0; k < blocks_needed; ++k)
            set_block_used(i + k);

        allocation_sizes[i] = static_cast<uint16_t>(blocks_needed);
        return &memory_pool_ptr[i * MEMORY_BLOCK_BYTE_SIZE];
    }

    bool Block_Allocator::fiend_enough_free_memory_blocks(const uint32_t i, 
                                                          const uint32_t blocks_needed,
                                                          uint32_t* j) noexcept {
        *j = 0;
        while (*j < blocks_needed) {
            if (!is_block_free(i + *j))
                return false;

            (*j)++;
        }

        return true;
    }

    bool Block_Allocator::get_allocation_info(void* ptr, 
                                              uint32_t& index, 
                                              uint32_t& blocks) noexcept {
        if ((ptr == nullptr) || (memory_pool_ptr == nullptr))
            return false;

        const uint8_t* block_ptr = reinterpret_cast<uint8_t*>(ptr);
        if (block_ptr < memory_pool_ptr ||
            block_ptr >= memory_pool_ptr + required_memory_pool_space)
            return false;

        const uint32_t offset = static_cast<uint32_t>(block_ptr - memory_pool_ptr);
        if (offset % MEMORY_BLOCK_BYTE_SIZE != 0)
            return false;

        index  = offset / MEMORY_BLOCK_BYTE_SIZE;
        blocks = allocation_sizes[index];
        if (blocks == 0)
            return false;

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

    [[nodiscard]]
    void* Block_Allocator::reallocate(void* ptr, const uint32_t new_size) noexcept {
        if (ptr == nullptr) [[unlikely]]
            return allocate(new_size);

        if (new_size == 0) [[unlikely]] {
            deallocate(ptr);
            return nullptr;
        }

        uint32_t index      = 0;
        uint32_t old_blocks = 0;
        if (!get_allocation_info(ptr, index, old_blocks)) [[unlikely]]
            sys::panic("Invalid realloc");

        const uint32_t old_size = old_blocks * MEMORY_BLOCK_BYTE_SIZE;

        void* new_ptr = allocate(new_size);
        if (!new_ptr)
            return nullptr;

        const uint32_t n = (old_size < new_size) 
                                   ? old_size 
                                   : new_size;
        runtime::Memory_Manipulation::copy_memory_block(new_ptr, ptr, n);

        deallocate(ptr);
        return new_ptr;
    }

    void Block_Allocator::deallocate(void* ptr) noexcept {
        uint32_t block_index = 0;
        uint32_t block_count = 0;

        if (!get_allocation_info(ptr, block_index, block_count))
            sys::panic("Invalid free");

        for (uint32_t k = 0; k < block_count; ++k)
            set_block_free(block_index + k);

        allocation_sizes[block_index] = 0;
    }
} // namespace kernel::heap
