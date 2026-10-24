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

#include <sal.hpp>
#include <types.hpp>
#include <status.hpp>


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
             _IN_ const uint8_t* const heap_end) noexcept;


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
        [[nodiscard]]
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
        clear_allocate(_OUT_ void*& block_ptr, 
                       _IN_  const uint32_t byte_size) noexcept;


        /** 
         * @brief Reallocates a memory block with a specific size and 
         *        preserves the previous contents up to the smaller size. 
         * 
         * @note Don't forget to free the allocated memory with `deallocate`, 
         *       otherwise memory leaks will occur. 
         * 
         * @param block_ptr     Pointer to the allocated memory block. 
         * @param new_byte_size New memory block byte size.
         * 
         * @retval `status::NULL_POINTER | status::flags::PARAM_A`
         *          If `block_ptr` is a `nullptr`.
         * 
         * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B` 
         *          If `new_byte_size` is 0. 
         * 
         * @retval `status::NULL_POINTER` 
         *          If the memory pool pointer inside the heap is empty.
         * 
         * @retval `status::POINTER_OUT_OF_RANGE` 
         *          If block_ptr is outside if the allocated memory block.
         * 
         * @retval `status::HEAP_EXHAUSTED` 
         *          If the memory pool does not have enough free space or
         *          `new_byte_size` is too large. 
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
        reallocate(_INOUT_ void*& block_ptr, 
                   _IN_    const uint32_t new_byte_size) noexcept;


        /** 
         * @brief Deallocates a memory block. 
         * 
         * @note If the deallocation was success, this nulls `block_ptr`.
         * 
         * @param block_ptr Pointer to the memory block to be deallocated.
         * 
         * @retval `status::NULL_POINTER` 
         *          If `block_ptr` is a `nullptr` or the 
         *          memory pool pointer inside the heap is empty. 
         * 
         * @retval `status::POINTER_OUT_OF_RANGE`
         *         If block_ptr is outside if the allocated memory block.
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
        deallocate(_INOUT_ void* block_ptr) noexcept;
} // namespace kernel::heap
