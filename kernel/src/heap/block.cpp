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
    /** 
     * @brief Sets all heap metadata and splits the 
     * heap area into a metadata area and a memory pool.
     *
     * @param start_address Heap area start address. 
     * @param end_address Heap area end address. 
     */
    void
    Block_Allocator::setup_metadata_layout(_IN_ uint32_t start_address,
                                           _IN_ uint32_t end_address) 
                                           noexcept {
        start_address = align_up(start_address, MEMORY_BLOCK_SIZE);
        end_address   = align_down(end_address, MEMORY_BLOCK_SIZE);

        if (end_address <= start_address) [[unlikely]] {
            sys::panic("Invalid memory range for heap initialization");
        }

        all_memory_blocks = (end_address - start_address) / MEMORY_BLOCK_SIZE;
        if (all_memory_blocks == 0) [[unlikely]] {
            sys::panic("Heap area too small");
        }

        const uint32_t allocation_sizes_bytes = align_up(all_memory_blocks * 
                                                         sizeof(uint16_t),
                                                         MEMORY_BLOCK_SIZE);

        const uint32_t bitmap_word_count = (all_memory_blocks + 
                                            BITMAP_WORD_MASK) / 
                                            BITMAP_WORD_BITS;
        const uint32_t bitmap_bytes      = align_up(bitmap_word_count * 
                                                    sizeof(uint32_t),
                                                    MEMORY_BLOCK_SIZE);

        const uint32_t metadata_start = start_address;
        const uint32_t bitmap_start   = metadata_start + allocation_sizes_bytes;
        const uint32_t pool_start     = align_up(bitmap_start + 
                                                 bitmap_bytes,
                                                 MEMORY_BLOCK_SIZE);

        if (pool_start >= end_address) [[unlikely]] {
            sys::panic("Heap metadata area too large");
        }

        allocation_sizes = reinterpret_cast<uint16_t*>(metadata_start);
        memory_bitmap    = reinterpret_cast<uint32_t*>(bitmap_start);
        memory_pool_ptr  = reinterpret_cast<uint8_t*>(pool_start);

        needed_pool_space = end_address - pool_start;
        needed_pool_space = align_down(needed_pool_space, MEMORY_BLOCK_SIZE);

        if (needed_pool_space == 0) [[unlikely]] {
            sys::panic("Heap area too small");
        }

        all_memory_blocks = needed_pool_space / MEMORY_BLOCK_SIZE;
    }


    /** 
     * @brief Resets all heap metadata. 
     * 
     * @param bitmap_word_count Count of available memory blocks in words. 
     */
    void
    Block_Allocator::clear_metadata(_IN_ const uint32_t bitmap_word_count) 
                                    noexcept {
        const uint32_t allocation_sizes_bytes = align_up(all_memory_blocks * 
                                                         sizeof(uint16_t),
                                                         MEMORY_BLOCK_SIZE);

        stdlib::Memory_Manipulation::set_memory_block(allocation_sizes,
                                                      MEMORY_CLEAR,
                                                      allocation_sizes_bytes);

        const uint32_t memory_bitmap_bytes = align_up(bitmap_word_count * 
                                                      sizeof(uint32_t),
                                                      MEMORY_BLOCK_SIZE);

        stdlib::Memory_Manipulation::set_memory_block(memory_bitmap,
                                                      ALL_BLOCKS_FREE,
                                                      memory_bitmap_bytes);
    }


    /**
     * @brief Marks memory blocks as used in the memory pool and 
     *        stores the memory block count.
     *
     * @param needed_blocks Number of needed blocks.
     * @param pool_index Start index in the memory pool.
     *
     * @return Pointer to the first block.
     */
    void*
    Block_Allocator::set_allocation_sizes_entry(_IN_ const uint32_t needed_blocks,
                                                _IN_ const uint32_t pool_index) noexcept {
        void* ptr;

        if (needed_blocks == 0 ||
            needed_blocks > UINT16_MAX ||
            pool_index >= all_memory_blocks ||
            needed_blocks > (all_memory_blocks - pool_index)) [[unlikely]] {
            ptr = nullptr;
            goto cleanup;
        }

        for (uint32_t pool_block  = 0; 
             pool_block < needed_blocks; 
             pool_block++) [[likely]] {
            set_block_used(pool_index + pool_block );
        }

        allocation_sizes[pool_index] = static_cast<uint16_t>(needed_blocks);

        ptr = &memory_pool_ptr[pool_index * MEMORY_BLOCK_SIZE];

    cleanup:
        return ptr;
    }


    /** 
     * @brief Checks free memory blocks in the memory pool from a start index.
     * 
     * @param checked_blocks Number of blocks checked so far.
     * @param start_index Start index in the memory pool.
     * @param needed_blocks Number of blocks needed.
     * 
     * @retval `true`  If enough free blocks were found.
     * @retval `false` If not enough free blocks were found.
     */
    bool 
    Block_Allocator::find_enough_free_blocks(_INOUT_ uint32_t& checked_memory_blocks,
                                             _IN_    const uint32_t pool_index,
                                             _IN_    const uint32_t needed_blocks) 
                                             noexcept {
        bool status;

        checked_memory_blocks = 0;
        while (checked_memory_blocks < needed_blocks) [[likely]] {
            if (!is_block_free(pool_index + 
                               checked_memory_blocks)) [[unlikely]] {
                status = false;
                goto cleanup;
            }   

            checked_memory_blocks++;
        }

        status = true;

    cleanup:
        return status;
    }


    /** 
     * @brief Shows how many memory blocks are needed for the
     *        current allocation.
     * 
     * @param needed_blocks count of needed memory blocks
     * @param byte_size allocation bytesize
     * 
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
     *          If `byte_size` is `0`.
     *
     * @retval `status::HEAP_EXHAUSTED`
     *          If the memory pool doesn't have enough free space
     *          or `byte_size` is to large.
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     */
    status_t 
    Block_Allocator::validate_allocate_size(_OUT_ uint32_t& needed_blocks,
                                            _IN_  const uint32_t byte_size) 
                                            noexcept {
        status_t status;
        
        if (byte_size == 0) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_B;
            goto cleanup;
        }

        needed_blocks = (byte_size + MEMORY_BLOCK_SIZE - 1) / 
                         MEMORY_BLOCK_SIZE;
        if (needed_blocks > all_memory_blocks || 
            needed_blocks > UINT16_MAX) [[unlikely]] {
            status = status::HEAP_EXHAUSTED;
            goto cleanup;
        }

        status = status::SUCCESS;

    cleanup:
        return status;
    }


    /**
     * @brief Finds a free memory region with a specific size
     *        in the heap memory pool. 
     * 
     * @param block_index Index of the found memory region. 
     * @param needed_blocks Count of all needed memory blocks. 
     * 
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B` 
     *          If `needed_blocks` is `0`. 
     * 
     * @retval `status::HEAP_EXHAUSTED` 
     *          If the memory pool does not have enough free space 
     *          or `needed_blocks` is too large. 
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     */
    status_t 
    Block_Allocator::find_free_memory_region(_OUT_ uint32_t& block_index,
                                             _IN_  const uint32_t needed_blocks)
                                             noexcept {
        status_t status;
        uint32_t pool_index         = 0;
        uint32_t memory_found_index = 0;

        if (needed_blocks == 0 || 
            needed_blocks > all_memory_blocks) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_B;
            goto cleanup;
        }

        while (pool_index <= 
               all_memory_blocks - needed_blocks) [[likely]] {
            if (find_enough_free_blocks(memory_found_index, 
                                        pool_index, 
                                        needed_blocks)) {
                block_index = pool_index;
                
                status = status::SUCCESS;
                goto cleanup;
            }

            pool_index += memory_found_index + 1;
        }

        status = status::HEAP_EXHAUSTED;

    cleanup:

        return status;
    }


    /** 
     * @brief Executes a allocation after the 
     *        parameter validation in `reallocate`.
     * 
     * @param block_ptr pointer to the allocated memory block
     * @param new_byte_size size of the memory block
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *          If `block_ptr` is a `nullptr`.
     * 
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
     *          If `new_byte_size` is `0`.
     *  
     * @retval `status::HEAP_EXHAUSTED`
     *          If the memory pool doesn't have enough free space
     *          or `new_byte_size` is to large.
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     */
    status_t 
    Block_Allocator::perform_reallocate(_INOUT_ void*& block_ptr,
                                        _IN_    const uint32_t new_byte_size)
                                        noexcept {
        uint32_t block_index       = 0;
        uint32_t old_memory_blocks = 0;
        void* new_block_ptr        = nullptr;
        status_t status;
        uint32_t old_byte_size;
        uint32_t memory_byte_size;

        if (!block_ptr) [[unlikely]] {
            status = status::NULL_POINTER | status::flags::PARAM_A;
            goto cleanup;
        }

        if (new_byte_size == 0) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_B;
            goto cleanup;
        }

        if (get_allocation_info(block_index, 
                                old_memory_blocks, 
                                new_block_ptr) != status::SUCCESS) [[unlikely]] {
            sys::panic("Invalid reallocate!");
        }
    
        status = allocate(new_block_ptr, new_byte_size);
        if (status != status::SUCCESS || !new_block_ptr) [[unlikely]] {
            goto cleanup;
        }
    
        old_byte_size = old_memory_blocks * MEMORY_BLOCK_SIZE;
        if (old_byte_size < new_byte_size) {
            memory_byte_size = old_byte_size;
        } 
        else {
            memory_byte_size = new_byte_size;
        }
    
        stdlib::Memory_Manipulation::copy_memory_block(new_block_ptr, 
                                                       block_ptr, 
                                                       memory_byte_size);
        deallocate(block_ptr);
    
        block_ptr = new_block_ptr;

        status = status::SUCCESS;
    
    cleanup:
        return status;
    }


    /** 
     * @brief Initializes the heap.
     * 
     * @param heap_begin memory pool start address
     * @param heap_end memory pool end address
     */
    _API_ 
    void
    Block_Allocator::init(_IN_ const uint8_t* heap_begin,
                          _IN_ const uint8_t* heap_end) noexcept {
        kernel::sys::disable_interrupts();

        setup_metadata_layout(reinterpret_cast<uint32_t>(heap_begin), 
                              reinterpret_cast<uint32_t>(heap_end));
        clear_metadata((all_memory_blocks + 31) / 32);

        kernel::sys::enable_interrupts();
    }


    /** 
     * @brief Gets the meta data of a allocated memory block.
     * 
     * @param pool_index Index of the allocated memory block in 
     *                          the memory pool.
     * @param pool_block_size Blocksize of the allocated memory block.
     * @param block_ptr pointer to the allocated memory block.
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_C`
     *          If `block_ptr` is a `nullptr`
     * 
     * @retval `status::NULL_POINTER`
     *          If the memory pool pointer inside the heap is empty.
     * 
     * @retval `status::POINTER_OUT_OF_RANGE`
     *          If `block_ptr` is outside if the allocated memory block.
     * 
     * @retval `status::HEAP_CORRUPTED`
     *          If the allocated memory block is not align.
     * 
     * @retval `status::FAIL`
     *          Unknown error.
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_ 
    status_t 
    Block_Allocator::get_allocation_info(_OUT_ uint32_t& pool_index,
                                         _OUT_ uint32_t& pool_block_size,
                                         _IN_  void* block_ptr) 
                                         noexcept {
        kernel::sys::disable_interrupts();

        status_t status;
        uint8_t* new_block_ptr;
        uint32_t offset;

        if (!block_ptr) [[unlikely]] {
            status = status::NULL_POINTER | status::flags::PARAM_C;
            goto cleanup;
        }

        if (!memory_pool_ptr) [[unlikely]] {
            status = status::NULL_POINTER;
            goto cleanup;
        }

        new_block_ptr = reinterpret_cast<uint8_t*>(block_ptr);
        if (new_block_ptr < memory_pool_ptr ||
            new_block_ptr >= memory_pool_ptr + needed_pool_space) [[unlikely]] {
            status = status::POINTER_OUT_OF_RANGE;
            goto cleanup;
        }

        offset = static_cast<uint32_t>(new_block_ptr - memory_pool_ptr);
        if (offset % MEMORY_BLOCK_SIZE != 0) [[unlikely]] {
            status = status::HEAP_CORRUPTED;
            goto cleanup;
        }

        pool_index = offset / MEMORY_BLOCK_SIZE;
        if (pool_index >= all_memory_blocks) [[unlikely]] {
            status = status::HEAP_CORRUPTED;
            goto cleanup;
        }

        pool_block_size = allocation_sizes[pool_index];
        if (pool_block_size == 0) [[unlikely]] {
            status = status::FAIL;
            goto cleanup;
        }

        status = status::SUCCESS;

    cleanup:
        kernel::sys::enable_interrupts();

        return status;
    }


    /**  
     * @brief Allocates a memory block with a specific size.
     * 
     * @note Don't forget to free the allocated memory with `deallocate`,
     *       otherwise memory leaks will occur.
     * 
     * @param block_ptr pointer to the allocated memory block
     * @param byte_size memory block byte size
     * 
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
     *          If `byte_size` is `0`.
     * 
     * @retval `status::HEAP_EXHAUSTED`
     *          If the memory pool doesn't have enough free space or 
     *          `byte_size` is to large.
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_ 
    status_t 
    Block_Allocator::allocate(_OUT_ void*& block_ptr, 
                              _IN_  const uint32_t byte_size) noexcept {
        status_t status;
        uint32_t needed_blocks;
        uint32_t block_index;

        if (!memory_pool_ptr || 
            !allocation_sizes || 
            !memory_bitmap) [[unlikely]] {
            sys::panic("'Block_allocator' not initialized");
        }

        status = validate_allocate_size(needed_blocks, 
                                        byte_size);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }
    
        kernel::sys::disable_interrupts();
        
        status = find_free_memory_region(block_index, 
                                         needed_blocks);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }
    
        block_ptr = set_allocation_sizes_entry(needed_blocks, 
                                               block_index);
        if (!block_ptr) [[unlikely]] {
            status = status::OUT_OF_MEMORY;
            goto cleanup;
        }

        status = status::SUCCESS;
    
        goto done;
    
    cleanup:
        block_ptr = nullptr;
    
    done:
        kernel::sys::enable_interrupts();

        return status;
    }


    /**  
     * @brief Allocates a memory block with a specific size and
     *        nulls all bytes in the allocated block.
     * 
     * @note Don't forget to free the allocated memory with `deallocate`,
     *       otherwise memory leaks will occur.
     * 
     * @param block_ptr pointer to the allocated memory block
     * @param byte_size memory block byte size
     * 
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
     *          If `byte_size` is `0`.
     * 
     * @retval `status::HEAP_EXHAUSTED`
     *          If the memory pool doesn't have enough free space or 
     *          `byte_size` is to large.
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_ 
    status_t 
    Block_Allocator::clear_allocate(_OUT_ void*& block_ptr, 
                                    _IN_  const uint32_t byte_size) 
                                    noexcept {
        status_t status;

        if (byte_size == 0) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_B;
            goto cleanup;
        }

        kernel::sys::disable_interrupts();

        status = allocate(block_ptr, byte_size);
        if (status != status::SUCCESS || !block_ptr) [[unlikely]] {
            goto cleanup;
        }

        stdlib::Memory_Manipulation::set_memory_block(block_ptr, 
                                                      MEMORY_CLEAR, 
                                                      byte_size);
        status = status::SUCCESS;

        goto success;

    cleanup: 
        block_ptr = nullptr;

    success: 
        kernel::sys::enable_interrupts();

        return status;
    }


    /** 
     * @brief Reallocates a memory block with a specific size and 
     *        preserves the previous contents up to the smaller size. 
     * 
     * @note Don't forget to free the allocated memory with `deallocate`, 
     *       otherwise memory leaks will occur. 
     * 
     * @param block_ptr Pointer to the allocated memory block. 
     * @param new_byte_size New memory block byte size.
     * 
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B` 
     *          If `new_byte_size` is 0. 
     * 
     * @retval `status::HEAP_EXHAUSTED` 
     *          If the memory pool does not have enough free space or
     *          `new_byte_size` is too large. 
     * 
     * @retval `status::SUCCESS` 
     *          Default case. 
     */
    _API_ 
    status_t 
    Block_Allocator::reallocate(_INOUT_ void*& block_ptr,
                                _IN_    const uint32_t new_byte_size) 
                                noexcept {
        status_t status;

        if (!allocation_sizes || !memory_bitmap) [[unlikely]] {
            sys::panic("'Block_allocator' not initialized");
        }

        kernel::sys::disable_interrupts();

        if (!block_ptr) [[unlikely]] {
            status = allocate(block_ptr, new_byte_size);
            goto cleanup;
        }

        if (new_byte_size == 0) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_B;
            goto cleanup;
        }

        status = perform_reallocate(block_ptr, new_byte_size);

    cleanup:
        kernel::sys::enable_interrupts();

        return status;
    }


    /** 
     * @brief Deallocates a memory block. 
     * 
     * @param block_ptr Pointer to the memory block to be deallocated.
     * 
     * @retval `status::NULL_POINTER` 
     *          If `block_ptr` is a `nullptr`. 
     * 
     * @retval `status::HEAP_CORRUPTED` 
     *          If a deallocation error has occurred. 
     * 
     * @retval `status::SUCCESS` 
     *          Default case. 
     */
    _API_
    status_t
    Block_Allocator::deallocate(_IN_ void* block_ptr) noexcept {
        status_t status;

        uint32_t block_index   = 0;
        uint32_t needed_blocks = 0;

        if (!allocation_sizes || !memory_bitmap) [[unlikely]] {
            sys::panic("'Block_allocator' not initialized");
        }

        if (!block_ptr) [[unlikely]] {
            status = status::NULL_POINTER;
            goto cleanup;
        }

        kernel::sys::disable_interrupts();

        if (get_allocation_info(block_index,
                                needed_blocks,
                                block_ptr) != status::SUCCESS) [[unlikely]] {
            sys::panic("Invalid free");
        }

        if (block_index + needed_blocks > 
            all_memory_blocks) [[unlikely]] {
            status = status::HEAP_CORRUPTED;
            goto cleanup;
        }

        for (uint32_t pool_block = 0; 
             pool_block < needed_blocks; 
             pool_block++) [[likely]] {
            set_block_free(block_index + pool_block);
        }

        allocation_sizes[block_index] = 0;

        status = status::SUCCESS;

    cleanup:
        kernel::sys::enable_interrupts();

        return status;
    }
} // namespace kernel::heap
