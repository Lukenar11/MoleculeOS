/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This library defines basic routines for memory manipulation
    that can be used sys-wide.

    The routines include copying, moving, comparing, and filling memory

NOTES:
    Because of the C API for the compiler, the methods are defined in the
    header so that the compiler can inline them.
*/


#pragma once

#include <status.hpp>
#include <sal.hpp>
#include <types.hpp>


namespace stdlib::memory
{
    /**
     * @brief Copys a memory-block with a specific byte_size.
     *
     * @param destination_ptr pointer to the destination memory-block
     * @param source_ptr      pointer to the source memory-block
     * @param byte_size       memory-block byte byte_size
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *          If the destination-pointer is `nullptr`.
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_B`
     *          If the source-pointer is `nullptr`.
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_  
    status_t 
    copy(_IN_ void* const destination_ptr, 
         _IN_ const void* const source_ptr, 
         _IN_ uint32_t byte_size) noexcept;


    /**
     * @brief Copys a memory-block with a specific byte_size and 
     *        alows backward copying.
     *
     * @param destination_ptr pointer to the destination memory-block
     * @param source_ptr      pointer to the source memory-block
     * @param byte_size       memory-block byte byte_size
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *          If the destination-pointer is `nullptr`.
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_B`
     *          If the source-pointer is `nullptr`.
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_  
    status_t 
    move(_IN_ void* const destination_ptr, 
         _IN_ const void* const source_ptr, 
         _IN_ uint32_t byte_size) noexcept;


    /**
     * @brief Fils a memory-block with 
     *        a specific byte_size with a specific value.
     *
     * @param destination_ptr pointer to the memory-block
     * @param value           value with which the memory-block 
     *                        has to be filed
     * @param byte_size       memory-block byte byte_size
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *          If `destination_ptr` is a `nullptr`.
     *
     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_  
    status_t 
    set_memory_block(_IN_ void* const destination_ptr, 
                     _IN_ const int32_t value, 
                     _IN_ uint32_t byte_size) noexcept;


    /**
     * @brief Compares the values in two diffenent memory-blocks 
     *        with a specific byte_size.
     *
     * @param destination_ptr pointer to the destination memory-block
     * @param source_ptr      pointer to the source memory-block
     * @param byte_size       memory-block byte byte_size
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *          If the destination-pointer is `nullptr`.
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_B`
     *          If the source-pointer is `nullptr`.
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     * 
     * @retval `status::GREATER_THAN`
     *          If the content of memory-block a is greater then the 
     *          content of memory-block b.
     *  
     * @retval `status::LESS_THAN`
     *          If the content of memory-block a is less then the 
     *          content of memory-block b.
     * 
     * @retval `status::EQUAL_TO`
     *          If the contents of both memory-blocks are identical.
     */
    _API_  
    status_t 
    compare_memory_block(_IN_ const void* const a_ptr, 
                         _IN_ const void* const b_ptr,
                         _IN_ uint32_t byte_size) noexcept;
} // namespace stdlib::memory
