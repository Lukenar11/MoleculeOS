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
    Some methods are placed in the header 
    because they are so small that the compiler can inline them.
*/

#include "heap/block.hpp"

namespace kernel::heap
{
    void Block_Allocator::init(_IN_ const uint8_t* begin, 
                               _IN_ const uint8_t* end) noexcept {
        const uint32_t start_addr = reinterpret_cast<uint32_t>(begin);
        const uint32_t end_addr   = reinterpret_cast<uint32_t>(end);

        if (end_addr <= start_addr) [[unlikely]] {
            sys::panic("Invalid memory range for heap initialization");
        }

        const uint32_t total_raw_bytes = end_addr - start_addr;
        all_memory_blocks              = total_raw_bytes / MEMORY_BLOCK_SIZE;
        if (all_memory_blocks == 0) [[unlikely]] {
            sys::panic("Heap area too small");
        }

        allocation_sizes   = reinterpret_cast<uint16_t*>(start_addr);
        free_memory_bitmap = reinterpret_cast<uint32_t*>(start_addr + 
                                                         all_memory_blocks * 
                                                         sizeof(uint16_t));

        const uint32_t bitmap_word_count = (all_memory_blocks + 31) / 32;
        uint32_t data_pool_start = start_addr + all_memory_blocks * 
                                   sizeof(uint16_t) + bitmap_word_count * 
                                   sizeof(uint32_t);

        data_pool_start = (data_pool_start + (MEMORY_BLOCK_SIZE - 1)) &
                          ~(MEMORY_BLOCK_SIZE - 1);

        needed_pool_space = all_memory_blocks * MEMORY_BLOCK_SIZE;
        if (data_pool_start + needed_pool_space > end_addr) {
            all_memory_blocks--;
            needed_pool_space = all_memory_blocks * MEMORY_BLOCK_SIZE;
        }

        memory_pool_ptr = reinterpret_cast<uint8_t*>(data_pool_start);

        stdlib::Memory_Manipulation::set_memory_block(free_memory_bitmap, 
                                                      0xFFFF'FFFF, 
                                                      bitmap_word_count * 
                                                      sizeof(uint32_t));
        
        stdlib::Memory_Manipulation::set_memory_block(allocation_sizes, 
                                                      0, 
                                                      all_memory_blocks * 
                                                      sizeof(uint16_t));
    }

    void* Block_Allocator::set_allocation_sizes_entry(_IN_ const uint32_t blocks_needed, 
                                                      _IN_ const uint32_t i) noexcept {
        for (uint32_t k = 0; k < blocks_needed; ++k) [[likely]] {
            set_block_used(i + k);
        }

        allocation_sizes[i] = static_cast<uint16_t>(blocks_needed);
        return &memory_pool_ptr[i * MEMORY_BLOCK_SIZE];
    }

    bool Block_Allocator::find_enough_free_memory_blocks(_INOUT_    uint32_t& j,
                                                         _IN_ const uint32_t i, 
                                                         _IN_ const uint32_t blocks_needed) 
                                                         noexcept {
        j = 0;
        while (j < blocks_needed) [[likely]] {
            if (!is_block_free(i + j)) [[unlikely]] {
                return false;
            }   

            j++;
        }

        return true;
    }

    bool Block_Allocator::get_allocation_info(_IN_  void* ptr, 
                                              _OUT_ uint32_t& index, 
                                              _OUT_ uint32_t& blocks) 
                                              noexcept {
        if (ptr == nullptr || memory_pool_ptr == nullptr) [[unlikely]] {
            return false;
        }

        const uint8_t* block_ptr = reinterpret_cast<uint8_t*>(ptr);
        if (block_ptr < memory_pool_ptr ||
            block_ptr >= memory_pool_ptr + 
            needed_pool_space) [[unlikely]] {
            return false;
        }

        const uint32_t offset = static_cast<uint32_t>(block_ptr - 
                                                      memory_pool_ptr);
        if (offset % MEMORY_BLOCK_SIZE != 0) [[unlikely]] {
            return false;
        }

        index  = offset / MEMORY_BLOCK_SIZE;
        blocks = allocation_sizes[index];
        if (blocks == 0) [[unlikely]] {
            return false;
        }

        return true;
    }

    [[nodiscard]]
    void* Block_Allocator::allocate(_IN_ const uint32_t size) noexcept {
        if (memory_pool_ptr == nullptr) [[unlikely]] {
            sys::panic("Block allocator not initialized");
        }

        if (size == 0) [[unlikely]] {
            return nullptr;
        }

        const uint32_t blocks_needed = (size + MEMORY_BLOCK_SIZE - 1) /
                                        MEMORY_BLOCK_SIZE;
        if (blocks_needed > all_memory_blocks) [[unlikely]] {
            return nullptr;
        }

        uint32_t i = 0;
        uint32_t j = 0;
        while (i <= all_memory_blocks - blocks_needed) [[likely]] {
            if (find_enough_free_memory_blocks(j, 
                                               i, 
                                               blocks_needed)) [[likely]] {
                return set_allocation_sizes_entry(blocks_needed, i);
            }
            else [[unlikely]] {
                i += j + 1;
            }
        }

        return nullptr;
    }

    void* Block_Allocator::clear_allocate(_IN_ const uint32_t size) noexcept {
        if (size == 0) [[unlikely]] {
            return nullptr;
        }
    
        void* ptr = allocate(size);
        if (ptr == nullptr) [[unlikely]] {
            return nullptr;
        }
    
        stdlib::Memory_Manipulation::set_memory_block(ptr, 0, size);
        return ptr;
    }

    [[nodiscard]]
    void* Block_Allocator::reallocate(_IN_ void* ptr, 
                                      _IN_ const uint32_t new_size) noexcept {
        if (ptr == nullptr) [[unlikely]] {
            return allocate(new_size);
        }

        if (new_size == 0) [[unlikely]] {
            deallocate(ptr);
            return nullptr;
        }

        uint32_t index      = 0;
        uint32_t old_blocks = 0;
        if (!get_allocation_info(ptr, index, old_blocks)) [[unlikely]] {
            sys::panic("Invalid realloc");
        }

        void* new_ptr = allocate(new_size);
        if (!new_ptr) [[unlikely]] {
            return nullptr;
        }

        const uint32_t old_size = old_blocks * MEMORY_BLOCK_SIZE;
        const uint32_t n        = (old_size < new_size) 
                                  ? old_size 
                                  : new_size;
        stdlib::Memory_Manipulation::copy_memory_block(new_ptr, ptr, n);

        deallocate(ptr);
        return new_ptr;
    }

    void Block_Allocator::deallocate(_IN_ void* ptr) noexcept {
        uint32_t block_index = 0;
        uint32_t block_count = 0;

        if (!get_allocation_info(ptr, block_index, block_count)) [[unlikely]] {
            sys::panic("Invalid free");
        }

        for (uint32_t k = 0; k < block_count; ++k) [[likely]] {
            set_block_free(block_index + k);
        }

        allocation_sizes[block_index] = 0;
    }
} // namespace kernel::heap
