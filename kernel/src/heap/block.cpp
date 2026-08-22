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
    void Block_Allocator::compute_block_count(_IN_ const uint32_t start_addr,
                                              _IN_ const uint32_t end_addr) 
                                              noexcept {
        const uint32_t total_raw_bytes = end_addr - start_addr;
        all_memory_blocks              = total_raw_bytes / MEMORY_BLOCK_SIZE;
        
        if (all_memory_blocks == 0) [[unlikely]] {
            sys::panic("Heap area too small");
        }
    }

    void Block_Allocator::setup_metadata_pointers(_IN_ const uint32_t start_addr) 
                                                  noexcept {
        allocation_sizes = reinterpret_cast<uint16_t*>(start_addr);
        memory_bitmap    = reinterpret_cast<uint32_t*>(start_addr + 
                                                       all_memory_blocks * 
                                                       sizeof(uint16_t));
    }

    void Block_Allocator::setup_memory_pool(_IN_ const uint32_t start_addr,
                                            _IN_ const uint32_t end_addr,
                                            _IN_ const uint32_t word_count) 
                                            noexcept {
        uint32_t data_pool_start = start_addr +
                                   all_memory_blocks * sizeof(uint16_t) +
                                   word_count * sizeof(uint32_t);

        needed_pool_space = all_memory_blocks * MEMORY_BLOCK_SIZE;
        data_pool_start   = (data_pool_start + (MEMORY_BLOCK_SIZE - 1)) & 
                            ~(MEMORY_BLOCK_SIZE - 1);

        if (data_pool_start + needed_pool_space > end_addr) {
            all_memory_blocks--;
            needed_pool_space = all_memory_blocks * MEMORY_BLOCK_SIZE;
        }

        memory_pool_ptr = reinterpret_cast<uint8_t*>(data_pool_start);
    }

    void Block_Allocator::clear_metadata(_IN_ const uint32_t word_count) 
                                         noexcept {
        const uint32_t memory_bitmap_size = word_count * 
                                            sizeof(uint32_t);
        stdlib::Memory_Manipulation::set_memory_block(memory_bitmap,
                                                      ALL_BLOCKS_FREE,
                                                      memory_bitmap_size);

        const uint32_t memory_blocks_count = all_memory_blocks * 
                                             sizeof(uint16_t);
        stdlib::Memory_Manipulation::set_memory_block(allocation_sizes,
                                                      MEMORY_CLEAR,
                                                      memory_blocks_count);
    }

    void* Block_Allocator::set_allocation_sizes_entry(_IN_ const uint32_t blocks_needed, 
                                                      _IN_ const uint32_t i) noexcept {
        for (uint32_t k = 0; k < blocks_needed; ++k) [[likely]] {
            set_block_used(i + k);
        }

        allocation_sizes[i] = static_cast<uint16_t>(blocks_needed);
        return &memory_pool_ptr[i * MEMORY_BLOCK_SIZE];
    }

    bool Block_Allocator::find_enough_free_memory_blocks(_INOUT_ uint32_t& j,
                                                         _IN_    const uint32_t i, 
                                                         _IN_    const uint32_t blocks) 
                                                         noexcept {
        j = 0;
        while (j < blocks) [[likely]] {
            if (!is_block_free(i + j)) [[unlikely]] {
                return false;
            }   

            j++;
        }

        return true;
    }

    status_t Block_Allocator::validate_allocate_size(_OUT_ uint32_t& blocks_needed,
                                                     _IN_  const uint32_t size) 
                                                     noexcept {
        status_t status;
        
        if (size == 0) [[unlikely]] {
            status = status::INVALID_PARAMETER;
            goto cleanup;
        }

        blocks_needed = (size + MEMORY_BLOCK_SIZE - 1) / MEMORY_BLOCK_SIZE;
        if (blocks_needed > all_memory_blocks) [[unlikely]] {
            status = status::HEAP_EXHAUSTED;
            goto cleanup;
        }

        status = status::SUCCESS;

    cleanup:
        return status;
    }

    status_t Block_Allocator::find_free_memory_region(_OUT_ uint32_t& index,
                                                      _IN_  const uint32_t blocks_needed)
                                                      noexcept {
        status_t status;
        uint32_t i = 0;
        uint32_t j = 0;

        while (i <= all_memory_blocks - blocks_needed) [[likely]] {
            if (find_enough_free_memory_blocks(j, 
                                               i, 
                                               blocks_needed)) [[likely]] {
                index  = i;
                status = status::SUCCESS;
                goto cleanup;
            }

            i += j + 1;
        }

        status = status::HEAP_EXHAUSTED;

    cleanup:
        return status;
    }

    status_t Block_Allocator::perform_reallocate(_INOUT_ void*& ptr,
                                                 _IN_    const uint32_t new_size)
                                                 noexcept {
        uint32_t index      = 0;
        uint32_t old_blocks = 0;
        void* new_ptr       = nullptr;
        status_t status;
        uint32_t old_size;
        uint32_t n;

        if (get_allocation_info(index, 
                                old_blocks, 
                                ptr) != status::SUCCESS) [[unlikely]] {
            sys::panic("Invalid reallocate!");
        }
    
        status = allocate(new_ptr, new_size);
        if (status != status::SUCCESS || !new_ptr) [[unlikely]] {
            goto cleanup;
        }
    
        old_size = old_blocks * MEMORY_BLOCK_SIZE;
        if (old_size < new_size) {
            n = old_size;
        } 
        else {
            n = new_size;
        };
    
        stdlib::Memory_Manipulation::copy_memory_block(new_ptr, ptr, n);
        deallocate(ptr);
    
        ptr    = new_ptr;
        status = status::SUCCESS;
    
    cleanup:
        return status;
    }

    void Block_Allocator::init(_IN_ const uint8_t* begin, 
                               _IN_ const uint8_t* end) noexcept  {
        const uint32_t start_addr = reinterpret_cast<uint32_t>(begin);
        const uint32_t end_addr   = reinterpret_cast<uint32_t>(end);

        // validate heap range
        if (end_addr <= start_addr) [[unlikely]] {
            sys::panic("Invalid memory range for heap initialization");
        }
        
        compute_block_count(start_addr, end_addr);

        const uint32_t bitmap_word_count = (all_memory_blocks + 31) / 32;

        setup_metadata_pointers(start_addr);
        setup_memory_pool(start_addr, end_addr, bitmap_word_count);
        clear_metadata(bitmap_word_count);
    }

    status_t Block_Allocator::get_allocation_info(_OUT_ uint32_t& index, 
                                                  _OUT_ uint32_t& blocks,
                                                  _IN_  void* ptr) noexcept {
        status_t status;
        uint8_t* block_ptr;
        uint32_t offset;

        if (!ptr || !memory_pool_ptr) [[unlikely]] {
            status = status::NULL_POINTER;
            goto cleanup;
        }

        block_ptr = reinterpret_cast<uint8_t*>(ptr);
        if (block_ptr < memory_pool_ptr ||
            block_ptr >= memory_pool_ptr + 
            needed_pool_space) [[unlikely]] {
            status = status::POINTER_OUT_OF_RANGE;
            goto cleanup;
        }

        offset = static_cast<uint32_t>(block_ptr - memory_pool_ptr);
        if (offset % MEMORY_BLOCK_SIZE != 0) [[unlikely]] {
            status = status::HEAP_CORRUPTED;
            goto cleanup;
        }

        index  = offset / MEMORY_BLOCK_SIZE;
        blocks = allocation_sizes[index];
        if (blocks == 0) [[unlikely]] {
            status = status::FAIL;
            goto cleanup;
        }

        status = status::SUCCESS;

    cleanup:
        return status;
    }

    status_t Block_Allocator::allocate(_OUT_ void*& ptr,
                                       _IN_  const uint32_t size) noexcept  {
        if (!memory_pool_ptr) [[unlikely]] {
            sys::panic("'Block_allocator' not initialized");
        }
    
        status_t status;
        uint32_t blocks_needed;
        uint32_t index;
    
        status = validate_allocate_size(blocks_needed, size);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }
    
        status = find_free_memory_region(index, blocks_needed);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }
    
        ptr = set_allocation_sizes_entry(blocks_needed, index);
        // if (!ptr) [[unlikely]] {
        //     status = status::FAIL;
        //     goto cleanup;
        // }

        status = status::SUCCESS;
    
        goto done;
    
    cleanup:
        ptr = nullptr;
    
    done:
        return status;
    }

    status_t Block_Allocator::clear_allocate(_OUT_ void*& ptr,
                                             _IN_  const uint32_t size) 
                                             noexcept {
        status_t status;

        if (size == 0) [[unlikely]] {
            status = status::INVALID_PARAMETER;
            goto cleanup;
        }

        status = allocate(ptr, size);
        if (status != status::SUCCESS || !ptr) [[unlikely]] {
            goto cleanup;
        }

        stdlib::Memory_Manipulation::set_memory_block(ptr, MEMORY_CLEAR, size);
        status = status::SUCCESS;

        goto success;

    cleanup: 
        ptr = nullptr;

    success: 
        return status;
    }

    status_t Block_Allocator::reallocate(_INOUT_ void*& ptr,
                                         _IN_    const uint32_t new_size) noexcept {
        status_t status;

        if (!ptr) [[unlikely]] {
            status = allocate(ptr, new_size);
            goto cleanup;
        }

        if (new_size == 0) [[unlikely]] {
            status = status::INVALID_PARAMETER;
            goto cleanup;
        }

        status = perform_reallocate(ptr, new_size);

    cleanup:
        return status;
    }

    status_t Block_Allocator::deallocate(_IN_ void* ptr) noexcept {
        status_t status;

        uint32_t block_index = 0;
        uint32_t block_count = 0;

        if (!ptr) [[unlikely]] {
            status = status::NULL_POINTER;
            goto cleanup;
        }

        if (get_allocation_info(block_index,
                                block_count,
                                ptr) != status::SUCCESS) [[unlikely]] {
            sys::panic("Invalid free");
        }

        for (uint32_t k = 0; k < block_count; ++k) [[likely]] {
            set_block_free(block_index + k);
        }

        allocation_sizes[block_index] = 0;

        status = status::SUCCESS;

    cleanup:
        return status;
    }
} // namespace kernel::heap
