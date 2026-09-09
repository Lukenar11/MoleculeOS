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


#pragma once

#include "heap/helpers.hpp"
#include <types.hpp>
#include <sal.hpp>
#include <kernel.hpp>
#include <memory_manip.hpp>
#include <status.hpp>


namespace kernel::heap
{
    class Block_Allocator final {
    private:
        static inline constexpr uint32_t ALL_BLOCKS_FREE   = 0xFFFF'FFFF;
        static inline constexpr uint32_t MEMORY_CLEAR      = 0;
        static inline constexpr uint32_t MEMORY_BLOCK_SIZE = 16;

        static inline constexpr uint32_t BITMAP_WORD_BITS  = 32;
        static inline constexpr uint32_t BITMAP_BIT        = 1u;
        static inline constexpr uint32_t BITMAP_WORD_SHIFT = 5;
        static inline constexpr uint32_t BITMAP_WORD_MASK  = BITMAP_WORD_BITS - 1;

        static inline uint32_t all_memory_blocks = 0;
        static inline uint32_t needed_pool_space = 0;

        static inline uint8_t* memory_pool_ptr   = nullptr;
        static inline uint16_t* allocation_sizes = nullptr;
        static inline uint32_t* memory_bitmap    = nullptr;

        
        /** 
         * @brief Marks a memory block in the memory poll block as used. 
         * 
         * @param block_index Index of the memory blocks to use.
         */
        static 
        inline 
        void
        set_block_used(_IN_ const uint32_t block_index) noexcept {
            const uint32_t bitmap_index = block_index >> BITMAP_WORD_SHIFT;
            const uint32_t used_block   = ~(BITMAP_BIT << 
                                            (block_index & BITMAP_WORD_MASK));

            memory_bitmap[bitmap_index] &= used_block;
        }


        /** 
         * @brief Marks a memory block in the memory poll block as free. 
         * 
         * @param block_index Index of the memory blocks to free. 
         */
        static 
        inline 
        void 
        set_block_free(_IN_ const uint32_t block_index) noexcept {
            const uint32_t bitmap_index = block_index >> BITMAP_WORD_SHIFT;
            const uint32_t free_block   = (BITMAP_BIT << 
                                           (block_index & BITMAP_WORD_MASK));

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
        static 
        inline 
        bool 
        is_block_free(_IN_ const uint32_t block_index) noexcept {
            const uint32_t bitmap_index = block_index >> BITMAP_WORD_SHIFT;
            const bool block_status     = (1u << 
                                           (block_index & BITMAP_WORD_MASK));

            return memory_bitmap[bitmap_index] & block_status;
        }


        /** 
         * @brief Aligns a value up. 
         * 
         * @param value Value to align. 
         * @param alignment Alignment value. 
         *
         * @return aligned value.
        */
        static 
        inline 
        uint32_t
        align_up(_IN_ const uint32_t value,
                 _IN_ const uint32_t alignment) noexcept {
            return (value + (alignment - 1)) & ~(alignment - 1);
        }


        /** 
         * @brief Aligns a value down. 
         * 
         * @param value Value to align. 
         * @param alignment Alignment value. 
         * 
         * @return Aligned value. 
         */
        static 
        inline 
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
         * @param end_address Heap area end address. 
         */
        static
        void
        setup_metadata_layout(_IN_ uint32_t start_address,
                              _IN_ uint32_t end_address) 
                              noexcept;


        /** 
         * @brief Resets all heap metadata. 
         * 
         * @param bitmap_word_count Count of available memory blocks in words.
         */
        static 
        void 
        clear_metadata(_IN_ const uint32_t bitmap_word_count) noexcept;


        /**
         * @brief Marks memory blocks as used in the memory pool and 
         *        stores the memory block count.
         *
         * @param needed_blocks Number of needed blocks.
         * @param pool_index Start index in the memory pool.
         *
         * @return Pointer to the first block.
         */
        static 
        void* 
        set_allocation_sizes_entry(_IN_ const uint32_t needed_blocks, 
                                   _IN_ const uint32_t pool_index) noexcept;


        /** 
         * @brief Checks free memory blocks in the 
         *        memory pool from a start index.
         * 
         * @param checked_blocks Number of blocks checked so far.
         * @param start_index Start index in the memory pool.
         * @param needed_blocks Number of blocks needed.
         * 
         * @retval `true`  If enough free blocks were found.
         * @retval `false` If not enough free blocks were found.
         */
        static 
        bool 
        find_enough_free_memory_blocks(_INOUT_ uint32_t& checked_blocks,
                                       _IN_    const uint32_t start_index, 
                                       _IN_    const uint32_t needed_blocks)
                                       noexcept;


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
        static 
        status_t 
        validate_allocate_size(_OUT_ uint32_t& needed_blocks,
                               _IN_  const uint32_t byte_size) noexcept;


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
        static 
        status_t 
        find_free_memory_region(_OUT_ uint32_t& block_index,
                                _IN_  const uint32_t blocks_needed) noexcept;


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
        static 
        status_t 
        perform_reallocate(_INOUT_ void*& block_ptr,
                           _IN_    const uint32_t new_byte_size) noexcept;


    public:
        /** 
         * @brief Initializes the heap.
         * 
         * @param heap_begin memory pool start address
         * @param heap_end memory pool end address
         */
        _API_ 
        static 
        void 
        init(_IN_ const uint8_t* heap_begin, 
             _IN_ const uint8_t* heap_end) noexcept;


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
        static 
        status_t 
        get_allocation_info(_OUT_ uint32_t& pool_index, 
                            _OUT_ uint32_t& pool_block_size,
                            _IN_  void* block_ptr) noexcept;


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
         *          If byte_size` is 0.
         * 
         * @retval `status::HEAP_EXHAUSTED`
         *          If the memory pool doesn't have enough free space or 
         *          `byte_size` is to large.
         * 
         * @retval `status::SUCCESS`
         *          Default case.
         */
        _API_ 
        static 
        status_t 
        allocate(_OUT_ void*& block_ptr, 
                 _IN_  const uint32_t byte_size) noexcept;


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
        static 
        status_t 
        clear_allocate(_OUT_ void*& block_ptr, 
                       _IN_  const uint32_t byte_size) noexcept;


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
        static 
        status_t 
        reallocate(_INOUT_ void*& block_ptr, 
                   _IN_    const uint32_t new_byte_size) noexcept;


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
        static 
        status_t 
        deallocate(_IN_ void* block_ptr) noexcept;


        Block_Allocator() noexcept  = default;
        ~Block_Allocator() noexcept = default;
    };
} // namespace kernel::heap
