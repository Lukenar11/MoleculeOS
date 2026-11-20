/**
 * @brief Defines a library for basic memory manipulation.
 *
 * @copyright Copyright (c) 2026 Lukenar11 (Luke Matthes)
 * @license:  https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
 * @author    Lukenar11 (Luke Matthes)
 *
 * @note
 */


#pragma once

#include <include/stdlib/status.hpp>
#include <include/stdlib/sal.hpp>
#include <include/stdlib/types.hpp>


namespace stdlib::memory
{
    /**
     * @brief Copys a memory block with a specific byte size.
     *
     * @param destination pointer to the destination memory block
     * @param source      pointer to the source memory block
     * @param byte_size   memory block byte byte size
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *          If `destination` is `nullptr`.
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_B`
     *          If `source` is `nullptr`.
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_  
    [[nodiscard]]
    status_t 
    copy_block(_IN_ void* const destination, 
               _IN_ const void* const source, 
               _IN_ const uint32_t byte_size) noexcept;


    /**
     * @brief Copys a memory block with a specific byte size and 
     *        alows backward copying.
     *
     * @param destination pointer to the destination memory block
     * @param source      pointer to the source memory block
     * @param byte_size   memory block byte size
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *          If `destination` is `nullptr`.
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_B`
     *          If `source` is `nullptr`.
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_  
    [[nodiscard]]
    status_t 
    move_block(_IN_ void* const destination,
               _IN_ const void* const source,
               _IN_ const uint32_t byte_size) noexcept;


    /**
     * @brief Fils a memory block with 
     *        a specific byte size with a specific value.
     *
     * @param destination pointer to the memory block
     * @param value       value with which the memory block 
     *                    has to be filed
     * @param byte_size   memory block byte byte size
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *          If `destination` is a `nullptr`.
     *
     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_  
    [[nodiscard]]
    status_t 
    set_block(_IN_ void* const destination, 
              _IN_ const int32_t value, 
              _IN_ const uint32_t byte_size) noexcept;


    /**
     * @brief Compares the values in two diffenent memory blocks 
     *        with a specific byte size.
     *
     * @param a         pointer to destination memory block a
     * @param b         pointer to source memory block b
     * @param byte_size memory block byte size
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *          If `a` is `nullptr`.
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_B`
     *          If `b` is `nullptr`.
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     * 
     * @retval `status::GREATER_THAN`
     *          If the content of memory block a is greater then the 
     *          content of memory block b.
     *  
     * @retval `status::LESS_THAN`
     *          If the content of memory block a is less then the 
     *          content of memory block b.
     * 
     * @retval `status::EQUAL_TO`
     *          If the contents of both memory blocks are identical.
     */
    _API_  
    [[nodiscard]]
    status_t 
    compare_block(_IN_ const void* const a, 
                  _IN_ const void* const b,
                  _IN_ uint32_t byte_size) noexcept;
} // namespace stdlib::memory
