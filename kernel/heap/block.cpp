/**
 * @brief Implements the system heap (block-allocator).
 *
 * @copyright Copyright (c) 2026 Lukenar11 (Luke Matthes)
 * @license:  https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
 * @author    Lukenar11 (Luke Matthes)
 *
 * @note The heap needed the linker defined symbols, 
 *       `heap_start` and `heap_end` for the initialisation.
 */


#include "block.hpp"

#include <include/kernel/sys.hpp>
#include <include/stdlib/memory.hpp>


namespace
{
    using namespace kernel;
    using namespace heap;


    constexpr uint32_t ALL_BLOCKS_FREE   = 0xFFFF'FFFF;
    constexpr uint32_t MEMORY_BLOCK_SIZE = 16;

    constexpr uint32_t BITMAP_WORD_BITS  = 32;
    constexpr uint32_t BITMAP_BIT        = 1;
    constexpr uint32_t BITMAP_WORD_SHIFT = 5;
    constexpr uint32_t BITMAP_WORD_MASK  = BITMAP_WORD_BITS - 1;

    constexpr uint8_t MEMORY_NULL = 0x00;

    uint32_t all_memory_blocks = 0;
    uint32_t needed_pool_space = 0;

    uint8_t* memory_pool_begin = nullptr;
    uint16_t* allocation_sizes = nullptr;
    uint32_t* memory_bitmap    = nullptr;


    /** 
     * @brief Marks a memory block in the memory poll block as used. 
     * 
     * @param block_index Index of the memory blocks to use.
     */
    inline 
    void
    set_block_used(_IN_ const uint32_t block_index) noexcept {
        const uint32_t bitmap_index = block_index >> BITMAP_WORD_SHIFT;
        const uint32_t used_block   = ~(BITMAP_BIT << (block_index & BITMAP_WORD_MASK));

        memory_bitmap[bitmap_index] &= used_block;
    }


    /** 
     * @brief Marks a memory block in the memory poll block as free. 
     * 
     * @param block_index Index of the memory blocks to free. 
     */
    inline 
    void 
    set_block_free(_IN_ const uint32_t block_index) noexcept {
        const uint32_t bitmap_index = block_index >> BITMAP_WORD_SHIFT;
        const uint32_t free_block   = (BITMAP_BIT << (block_index & BITMAP_WORD_MASK));

        memory_bitmap[bitmap_index] |= free_block;
    }


    /** 
     * @brief Checks whether a memory block in the memory pool is free.
     * 
     * @param block_index Memory block index.
     * 
     * @retval `true`  
     *          If the memory block is free. 
     * 
     * @retval `false` 
     *          If the ,emory block is used. 
     */
    [[nodiscard]]
    inline 
    bool 
    is_block_free(_IN_ const uint32_t block_index) noexcept {
        const uint32_t bitmap_index = block_index >> BITMAP_WORD_SHIFT;
        const bool block_status     = (1U << (block_index & BITMAP_WORD_MASK));

        return memory_bitmap[bitmap_index] & block_status;
    }


    /** 
     * @brief Aligns a value up. 
     * 
     * @param value     Value to align. 
     * @param alignment Alignment value. 
     *
     * @return aligned value.
     */
    [[nodiscard]]
    inline 
    constexpr
    uint32_t
    align_up(_IN_ const uint32_t value,
             _IN_ const uint32_t alignment) noexcept {
        return (value + (alignment - 1)) & ~(alignment - 1);
    }


    /** 
     * @brief Aligns a value down. 
     * 
     * @param value q   Value to align. 
     * @param alignment Alignment value. 
     * 
     * @return Aligned value. 
     */
    [[nodiscard]]
    inline 
    constexpr
    uint32_t
    align_down(_IN_ const uint32_t value,
               _IN_ const uint32_t alignment) noexcept {
        return value & ~(alignment - 1);
    }

    /** 
     * @brief Sets all heap metadata and splits the 
     * heap area into a metadata area and a memory pool.
     *
     * @param start_address Heap area start address. 
     * @param end_address   Heap area end address. 
     */
    void 
    setup_metadata_layout(_IN_ uint32_t start_address,
                          _IN_ uint32_t end_address) noexcept {
        uint32_t allocation_sizes_bytes = 0;
        uint32_t bitmap_word_count      = 0;
        uint32_t bitmap_byte_size       = 0;
        uint32_t metadata_start         = 0;
        uint32_t memory_bitmap_start    = 0;
        uint32_t memory_pool_start      = 0;

        start_address = align_up(start_address, MEMORY_BLOCK_SIZE);
        end_address   = align_down(end_address, MEMORY_BLOCK_SIZE);

        if (end_address <= start_address) [[unlikely]] {
            sys::panic("Invalid memory range for heap initialization");
        }

        all_memory_blocks = (end_address - start_address) / MEMORY_BLOCK_SIZE;
        if (all_memory_blocks == 0) [[unlikely]] {
            sys::panic("Heap area too small");
        }

        allocation_sizes_bytes = align_up(all_memory_blocks * sizeof(uint16_t),
                                          MEMORY_BLOCK_SIZE);

        bitmap_word_count = (all_memory_blocks + BITMAP_WORD_MASK) / 
                             BITMAP_WORD_BITS;
    
        bitmap_byte_size = align_up(bitmap_word_count * sizeof(uint32_t), 
                                    MEMORY_BLOCK_SIZE);

        metadata_start      = start_address;
        memory_bitmap_start = metadata_start + allocation_sizes_bytes;
        memory_pool_start   = align_up(memory_bitmap_start + bitmap_byte_size, 
                                       MEMORY_BLOCK_SIZE);

        if (memory_pool_start >= end_address) [[unlikely]] {
            sys::panic("Heap metadata area too large");
        }

        allocation_sizes  = reinterpret_cast<uint16_t*>(metadata_start);
        memory_bitmap     = reinterpret_cast<uint32_t*>(memory_bitmap_start);
        memory_pool_begin = reinterpret_cast<uint8_t*>(memory_pool_start);

        needed_pool_space = end_address - memory_pool_start;
        needed_pool_space = align_down(needed_pool_space, MEMORY_BLOCK_SIZE);

        if (needed_pool_space == 0) [[unlikely]] {
            sys::panic("Heap area too small");
        }

        all_memory_blocks = needed_pool_space / MEMORY_BLOCK_SIZE;
    }


    /** 
     * @brief Resets all heap metadata. 
     * 
     * @param available_memory_blocks 
     *        Count of available memory blocks in words.
     */
    [[nodiscard]]
    status_t
    clear_metadata(_IN_ const uint32_t available_memory_blocks) noexcept {
        status_t status                  = status::SUCCESS;
        uint32_t allocable_bytes         = 0;
        uint32_t memory_bitmap_byte_size = 0;

        allocable_bytes = align_up(all_memory_blocks * sizeof(uint16_t), 
                                   MEMORY_BLOCK_SIZE);

        status = stdlib::memory::set_block(allocation_sizes, 
                                           MEMORY_NULL, 
                                           allocable_bytes);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        memory_bitmap_byte_size = align_up(available_memory_blocks * 
                                           sizeof(uint32_t),
                                           MEMORY_BLOCK_SIZE);

        status = stdlib::memory::set_block(memory_bitmap, 
                                           ALL_BLOCKS_FREE, 
                                           memory_bitmap_byte_size);

    cleanup:
        return status;
    }


    /**
     * @brief Marks memory blocks as used in the memory pool and 
     *        stores the memory block count.
     *
     * @param needed_memory_blocks Number of needed blocks.
     * @param memory_pool_index    Start index in the memory pool.
     *
     * @return Pointer to the first block.
     */ 
    void*
    set_allocation_sizes_entry(_IN_ const uint32_t needed_memory_blocks,
                               _IN_ const uint32_t memory_pool_index) 
                               noexcept {
        void* used_memory_marker = nullptr;

        if (needed_memory_blocks == 0 ||
            needed_memory_blocks > stdlib::UINT16_MAX ||
            needed_memory_blocks > (all_memory_blocks - memory_pool_index) ||
            memory_pool_index >= all_memory_blocks) [[unlikely]] {
            goto cleanup;
        }

        for (uint32_t pool_block  = 0; 
             pool_block < needed_memory_blocks; 
             pool_block++) [[likely]] {
            set_block_used(memory_pool_index + pool_block );
        }

        allocation_sizes [memory_pool_index] = 
            static_cast<uint16_t>(needed_memory_blocks);

        used_memory_marker = &memory_pool_begin[memory_pool_index * 
                                                MEMORY_BLOCK_SIZE];

    cleanup:
        return used_memory_marker;
    }


    /** 
     * @brief Checks free memory blocks in the memory pool from a start index.
     * 
     * @param checked_memory_blocks Number of blocks checked so far.
     * @param memory_pool_index     Start index in the memory pool.
     * @param needed_memory_blocks  Number of blocks needed.
     * 
     * @retval `true`  
     *          If enough free blocks were found.
     * 
     * @retval `false` 
     *          If not enough free blocks were found.
     */
    [[nodiscard]]
    bool 
    find_enough_free_blocks(_INOUT_ uint32_t& checked_memory_blocks,
                            _IN_    const uint32_t memory_pool_index,
                            _IN_    const uint32_t needed_memory_blocks) 
                            noexcept {
        bool status = true;

        checked_memory_blocks = 0;

        while (checked_memory_blocks < needed_memory_blocks) [[likely]] {
            if (!is_block_free(memory_pool_index + 
                               checked_memory_blocks)) [[unlikely]] {
                status = false;
                goto cleanup;
            }   

            checked_memory_blocks++;
        }

    cleanup:
        return status;
    }


    /** 
     * @brief Shows how many memory blocks are needed for the
     *        current allocation.
     * 
     * @param needed_blocks count of needed memory blocks
     * @param byte_size     allocation bytesize
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
    [[nodiscard]]
    status_t 
    validate_allocate_size(_OUT_ uint32_t& needed_memory_blocks,
                           _IN_  const uint32_t byte_size) noexcept {
        status_t status = status::SUCCESS;
        
        if (byte_size == 0) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_B;
            goto cleanup;
        }

        needed_memory_blocks = (byte_size + MEMORY_BLOCK_SIZE - 1) / 
                                MEMORY_BLOCK_SIZE;
        if (needed_memory_blocks > all_memory_blocks || 
            needed_memory_blocks > stdlib::UINT16_MAX) [[unlikely]] {
            status = status::HEAP_EXHAUSTED;
            goto cleanup;
        }

    cleanup:
        return status;
    }


    /**
     * @brief Finds a free memory region with a specific size
     *        in the heap memory pool. 
     * 
     * @param memory_region_index  Index of the found memory region. 
     * @param needed_memory_blocks Count of all needed memory blocks. 
     * 
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B` 
     *          If `needed_blocks` is `0`. 
     * 
     * @retval `status::HEAP_EXHAUSTED` 
     *          If the memory pool does not have enough free space 
     *          or `needed_blocks` is too large. 
     * 
     * @retval `status::SUCCESS`
     *            Default case.
     */
    [[nodiscard]]
    status_t 
    find_free_memory_region(_OUT_ uint32_t& memory_region_index,
                            _IN_  const uint32_t needed_memory_blocks) 
                            noexcept {
        status_t status            = status::HEAP_EXHAUSTED;
        uint32_t memory_pool_index = 0;
        uint32_t found_free_memory = 0;

        memory_region_index = 0;

        if (needed_memory_blocks == 0 || 
            needed_memory_blocks > all_memory_blocks) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_B;
            goto cleanup;
        }

        while (memory_pool_index <= 
               all_memory_blocks - needed_memory_blocks) [[likely]] {
            if (find_enough_free_blocks(found_free_memory, 
                                        memory_pool_index, 
                                        needed_memory_blocks)) {
                memory_region_index = memory_pool_index;
                status              = status::SUCCESS;

                goto cleanup;
            }

            memory_pool_index += found_free_memory + 1;
        }

    cleanup:
        return status;
    }


    /** 
     * @brief Gets the meta data of a allocated memory block.
     * 
     * @param memory_poll_index  Index of the allocated memory block in 
     *                           the memory pool.
     * @param memory_block_count Blocksize of the allocated memory block.
     * @param memory_block       Pointer to the allocated memory block.
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_C`
     *          If `memory_block` is a `nullptr`
     * 
     * @retval `status::NULL_POINTER`
     *          If the memory pool pointer inside the heap is empty.
     * 
     * @retval `status::POINTER_OUT_OF_RANGE`
     *          If `memory_block` is outside if the allocated memory block.
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
    [[nodiscard]]
    status_t 
    get_allocation_info(_OUT_ uint32_t& memory_poll_index,
                        _OUT_ uint32_t& memory_block_count,
                        _IN_  void* memory_block) noexcept {
        status_t status                  = status::SUCCESS;
        uint8_t* allocated_memory_block  = nullptr;
        uint32_t allocated_memory_offset = 0;

        memory_poll_index  = 0;
        memory_block_count = 0;

        if (!memory_block) [[unlikely]] {
            status = status::NULL_POINTER | status::flags::PARAM_C;
            goto cleanup;
        }

        if (!memory_pool_begin) [[unlikely]] {
            status = status::NULL_POINTER;
            goto cleanup;
        }

        allocated_memory_block = reinterpret_cast<uint8_t*>(memory_block);
        if (allocated_memory_block < memory_pool_begin ||
            allocated_memory_block >= memory_pool_begin + 
                                      needed_pool_space) [[unlikely]] {
            status = status::POINTER_OUT_OF_RANGE;
            goto cleanup;
        }

        allocated_memory_offset = static_cast<uint32_t>(allocated_memory_block - 
                                                        memory_pool_begin);
        if (allocated_memory_offset % MEMORY_BLOCK_SIZE != 0) [[unlikely]] {
            status = status::HEAP_CORRUPTED;
            goto cleanup;
        }

        memory_poll_index = allocated_memory_offset / MEMORY_BLOCK_SIZE;
        if (memory_poll_index >= all_memory_blocks) [[unlikely]] {
            status = status::HEAP_CORRUPTED;
            goto cleanup;
        }

        memory_block_count = allocation_sizes[memory_poll_index];
        if (memory_block_count == 0) [[unlikely]] {
            status = status::FAIL;
        }

    cleanup:
        return status;
    }


    /** 
     * @brief Executes a allocation after the 
     *        parameter validation in `reallocate`.
     * 
     * @param memory_block  pointer to the allocated memory block
     * @param new_byte_size size of the memory block
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *          If `memory_block` is a `nullptr`.
     * 
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
     *          If `new_byte_size` is `0`.
     * 
     * @retval `status::NULL_POINTER` 
     *          If the memory pool pointer inside the heap is empty.
     * 
     * @retval `status::POINTER_OUT_OF_RANGE` 
     *          If memory_block is outside if the allocated memory block.
     *  
     * @retval `status::HEAP_EXHAUSTED`
     *          If the memory pool doesn't have enough free space
     *          or `new_byte_size` is to large.
     * 
     * @retval `status::HEAP_CORRUPTED` 
     *          If the allocated memory block is not align.
     * 
     * @retval `status::OUT_OF_MEMORY`
     *         If the heap can't mark memory blocks as used.
     * 
     * @retval `status::FAIL` 
     *          Unknown error.
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     */
    [[nodiscard]]
    status_t 
    perform_reallocate(_INOUT_ void*& memory_block,
                       _IN_    const uint32_t new_byte_size) noexcept {
        using namespace stdlib;

        status_t status                 = status::SUCCESS;
        uint32_t old_memory_block_index = 0;
        uint32_t old_memory_block_size  = 0;
        void* new_memory_block          = nullptr;
        uint32_t old_block_byte_size    = 0;
        uint32_t memory_block_byte_size = 0;

        if (!memory_block) [[unlikely]] {
            status = status::NULL_POINTER | status::flags::PARAM_A;
            goto cleanup;
        }

        if (new_byte_size == 0) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_B;
            goto cleanup;
        }

        status = get_allocation_info(old_memory_block_index, 
                                     old_memory_block_size, 
                                     memory_block);
        if (status != status::SUCCESS) [[unlikely]] {
            if (status == (status::NULL_POINTER | status::flags::PARAM_C)) {
                // The `PARAM_C` flag is removed, so that the 
                // returned status code matches the function signature.

                status = status::NULL_POINTER;
            }

            goto cleanup;
        }

        status = allocate(new_memory_block, new_byte_size);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }
    
        old_block_byte_size = old_memory_block_size * MEMORY_BLOCK_SIZE;
        if (old_block_byte_size < new_byte_size) {
            memory_block_byte_size = old_block_byte_size;
        } 
        else {
            memory_block_byte_size = new_byte_size;
        }
    
        status = stdlib::memory::copy_block(new_memory_block, 
                                            memory_block, 
                                            memory_block_byte_size);
        if (status != status::SUCCESS) [[unlikely]] {
            status = status::FAIL;
            goto cleanup;
        }

        status = deallocate(memory_block);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }
    
        memory_block = new_memory_block;
    
    cleanup:
        return status;
    }    
} // namespace


namespace kernel::heap
{
    /** 
     * @brief Initializes the heap.
     * 
     * @param heap_begin memory pool start address
     * @param heap_end   memory pool end address
     */
    _API_ 
    void
    init(_IN_ const uint8_t* const heap_begin,
         _IN_ const uint8_t* const heap_end) noexcept {
        setup_metadata_layout(reinterpret_cast<uint32_t>(heap_begin), 
                              reinterpret_cast<uint32_t>(heap_end));
  
        if (clear_metadata((all_memory_blocks + BITMAP_WORD_BITS - 1) / 
                            BITMAP_WORD_BITS) != status::SUCCESS) [[unlikely]] {
            sys::panic("'Block_Allocator' init failed!");
        };

        if (!memory_pool_begin || 
            !allocation_sizes || 
            !memory_bitmap) [[unlikely]] {
            sys::panic("'Block_Allocator' not initialized!");
        }
    }


    /**  
     * @brief Allocates a memory block with a specific size.
     * 
     * @note Don't forget to free the allocated memory with `deallocate`,
     *       otherwise memory leaks will occur.
     * 
     * @param memory_block pointer to the allocated memory block
     * @param byte_size    memory block byte size
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
    [[nodiscard]]
    status_t 
    allocate(_OUT_ void*& memory_block, 
             _IN_  const uint32_t byte_size) noexcept {
        status_t status                  = status::SUCCESS;
        uint32_t needed_memory_blocks    = 0;
        uint32_t free_memory_block_index = 0;

        sys::disable_interrupts();

        memory_block = nullptr;

        status = validate_allocate_size(needed_memory_blocks, byte_size);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }
    
        status = find_free_memory_region(free_memory_block_index, 
                                         needed_memory_blocks);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }
    
        memory_block = set_allocation_sizes_entry(needed_memory_blocks, 
                                                  free_memory_block_index);
        if (!memory_block) [[unlikely]] {
            status = status::OUT_OF_MEMORY;
            goto cleanup;
        }

    cleanup:
        sys::enable_interrupts();

        return status;
    }


    /**  
     * @brief Allocates a memory block with a specific size and
     *        nulls all bytes in the allocated block.
     * 
     * @note Don't forget to free the allocated memory with `deallocate`,
     *       otherwise memory leaks will occur.
     * 
     * @param memory_block pointer to the allocated memory block
     * @param byte_size    memory block byte size
     * 
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
     *          If `byte_size` is `0`.
     * 
     * @retval `status::HEAP_EXHAUSTED`
     *          If the memory pool doesn't have enough free space or 
     *          `byte_size` is to large.
     * 
     * @retval `status::OUT_OF_MEMORY`
     *          If the heap can't mark memory blocks as used.
     * 
     * @retval `status::FAIL` 
     *          Unknown error.
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_ 
    [[nodiscard]]
    status_t
    clear_allocate(_OUT_ void*& memory_block, 
                   _IN_  const uint32_t byte_size) noexcept {
        status_t status                 = status::SUCCESS;
        uint32_t memory_block_byte_size = 0; 

        sys::disable_interrupts();

        memory_block = nullptr;

        if (byte_size == 0) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_B;
            goto cleanup;
        }

        sys::enable_interrupts();

        status = allocate(memory_block, byte_size);

        sys::disable_interrupts();

        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        memory_block_byte_size = align_up(byte_size, MEMORY_BLOCK_SIZE);

        status = stdlib::memory::set_block(memory_block, 
                                           MEMORY_NULL, 
                                           memory_block_byte_size);
        if (status != status::SUCCESS) [[unlikely]] {
            status = status::FAIL;
            goto cleanup;
        }

    cleanup: 
        sys::enable_interrupts();

        return status;
    }


    /** 
     * @brief Reallocates a memory block with a specific size and 
     *        preserves the previous contents up to the smaller size. 
     * 
     * @note Don't forget to free the allocated memory with `deallocate`, 
     *       otherwise memory leaks will occur. 
     * 
     * @param memory_block Pointer to the allocated memory block. 
     * @param byte_size    New memory block byte size.
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *          If `memory_block` is a `nullptr`.
     * 
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B` 
     *          If `byte_size` is 0. 
     * 
     * @retval `status::NULL_POINTER` 
     *          If the memory pool pointer inside the heap is empty.
     * 
     * @retval `status::POINTER_OUT_OF_RANGE` 
     *          If `memory_block` is outside if the allocated memory block.
     * 
     * @retval `status::HEAP_EXHAUSTED` 
     *          If the memory pool does not have enough free space or
     *          `byte_size` is too large. 
     * 
     * @retval `status::HEAP_CORRUPTED` 
     *          If the allocated memory block is not align.
     * 
     * @retval `status::OUT_OF_MEMORY`
     *          If the heap can't mark memory blocks as used.
     * 
     * @retval `status::FAIL` 
     *          Unknown error.
     * 
     * @retval `status::SUCCESS` 
     *          Default case. 
     */
    _API_ 
    [[nodiscard]]
    status_t
    reallocate(_INOUT_ void*& memory_block,
               _IN_    const uint32_t byte_size) noexcept {
        status_t status = status::SUCCESS;

        sys::disable_interrupts();

        if (!memory_block) [[unlikely]] {
            status = allocate(memory_block, byte_size);
            goto cleanup;
        }

        if (byte_size == 0) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_B;
            goto cleanup;
        }

        status = perform_reallocate(memory_block, byte_size);

    cleanup:
        sys::enable_interrupts();

        return status;
    }


    /** 
     * @brief Deallocates a memory block. 
     * 
     * @note If the deallocation was success, this nulls `memory_block`.
     * 
     * @param memory_block Pointer to the memory block to be deallocated.
     * 
     * @retval `status::NULL_POINTER` 
     *          If `memory_block` is a `nullptr` or the 
     *          memory pool pointer inside the heap is empty. 
     * 
     * @retval `status::POINTER_OUT_OF_RANGE`
     *         If memory_block is outside if the allocated memory block.
     * 
     * @retval `status::HEAP_CORRUPTED` 
     *          If a deallocation error has occurred. 
     * 
     * @retval `status::FAIL` 
     *          Unknown error.
     * 
     * @retval `status::SUCCESS` 
     *          Default case. 
     */
    _API_
    [[nodiscard]]
    status_t
    deallocate(_INOUT_ void*& memory_block) noexcept {
        status_t status               = status::SUCCESS;
        uint32_t memory_block_index   = 0;
        uint32_t needed_memory_blocks = 0;

        sys::disable_interrupts();

        if (!memory_block) [[unlikely]] {
            status = status::NULL_POINTER;
            goto cleanup;
        }

        status = get_allocation_info(memory_block_index, 
                                     needed_memory_blocks, 
                                     memory_block);
        if (status != status::SUCCESS) [[unlikely]] {
            if (status == (status::NULL_POINTER | status::flags::PARAM_C)) {
                // The `PARAM_C` flag is removed, so that the 
                // returned status code matches the function signature.

                status = status::NULL_POINTER;
            }

            goto cleanup;
        }

        if (memory_block_index + needed_memory_blocks > 
            all_memory_blocks) [[unlikely]] {
            status = status::HEAP_CORRUPTED;
            goto cleanup;
        }

        for (uint32_t block = 0; 
             block < needed_memory_blocks; 
             block++) [[likely]] {
            set_block_free(memory_block_index + block);
        }

        allocation_sizes[memory_block_index] = 0;
        memory_block                         = nullptr;

    cleanup:
        sys::enable_interrupts();

        return status;
    }
} // namespace kernel::heap
