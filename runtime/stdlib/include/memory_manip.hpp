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


namespace stdlib
{
    class Memory_Manipulation final {
    private:
        /**
         * @brief Validates the parameters for the most class methods.
         * 
         * @param dest_ptr pointer to the validate
         * @param src_ptr  pointer to the validate
         * @param size     byte size to validate
         * 
         * @retval `status::NULL_POINTER | status::flags::PARAM_A`
         *          If `dest_ptr` is `nullptr`.
         * 
         * @retval `status::NULL_POINTER | status::flags::PARAM_B`
         *          If `src_ptr` is `nullptr`.
         * 
         * @retval `status::SUCCESS | status::flags::SIZE_ZERO`
         *          If all parameters are valid but `size` is `0`.
         * 
         * @retval `status::SUCCESS`
         *          All parameters are valid.
         */
        [[nodiscard]] static status_t
        validate_parameters(_IN_ void* dest_ptr, 
                            _IN_ const void* src_ptr, 
                            _IN_ uint32_t size) noexcept;

                            
    public:
        /**
         * @brief Copys a memory-block with a specific size.
         *
         * @param dest_ptr pointer to the destination memory-block
         * @param src_ptr  pointer to the source memory-block
         * @param size     memory-block byte size
         *
         * @retval `status::NULL_POINTER | status::flags::PARAM_A`
         *          If the destination-pointer is `nullptr`.
         * 
         * @retval `status::NULL_POINTER | status::flags::PARAM_B`
         *          If the source-pointer is `nullptr`.
         * 
         * @retval `status::SUCCESS | status::flags::SIZE_ZERO`
         *          If the memory-block-byte size is `0`.
         * 
         * @retval `status::SUCCESS`
         *          Default case.
         */
        _API_ static status_t 
        copy_memory_block(_IN_ void* dest_ptr, 
                          _IN_ const void* src_ptr, 
                          _IN_ uint32_t size) noexcept;


        /**
         * @brief Copys a memory-block with a specific size and 
         *        alows backward copying.
         *
         * @param dest_ptr pointer to the destination memory-block
         * @param src_ptr  pointer to the source memory-block
         * @param size     memory-block byte size
         *
         * @retval `status::NULL_POINTER | status::flags::PARAM_A`
         *          If the destination-pointer is `nullptr`.
         * 
         * @retval `status::NULL_POINTER | status::flags::PARAM_B`
         *          If the source-pointer is `nullptr`.
         * 
         * @retval `status::SUCCESS | status::flags::SIZE_ZERO`
         *          If the memory-block-byte size is `0`.
         * 
         * @retval `status::SUCCESS`
         *          Default case.
         */
        _API_ static status_t 
        move_memory_block(_IN_ void* dest_ptr, 
                          _IN_ const void* src_ptr, 
                          _IN_ uint32_t size) noexcept;


        /**
         * @brief Fils a memory-block with a specific size with a specific value.
         *
         * @param dest_ptr pointer to the memory-block
         * @param value    value with which the memory-block has to be filed
         * @param size     memory-block byte size
         *
         * @retval `status::NULL_POINTER | status::flags::PARAM_A`
         *          If `dest_ptr` is a `nullptr`.
         * 
         * @retval `status::SUCCESS | status::flags::SIZE_ZERO`
         *          If the memory-block-byte size is `0`.
         * 
         * @retval `status::SUCCESS`
         *          Default case.
         */
        _API_ static status_t 
        set_memory_block(_IN_ void* dest_ptr, 
                         _IN_ const int32_t value, 
                         _IN_ uint32_t size) noexcept;


        /**
         * @brief Compares the values in two diffenent memory-blocks 
         *        with a specific size.
         *
         * @param dest_ptr pointer to the destination memory-block
         * @param src_ptr  pointer to the source memory-block
         * @param size     memory-block byte size
         *
         * @retval `status::NULL_POINTER | status::flags::PARAM_A`
         *          If the destination-pointer is `nullptr`.
         * 
         * @retval `status::NULL_POINTER | status::flags::PARAM_B`
         *          If the source-pointer is `nullptr`.
         * 
         * @retval `status::SUCCESS | status::flags::SIZE_ZERO`
         *          If the memory-block-byte size is `0`.
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
        _API_ static status_t 
        compare_memory_block(_IN_ const void* a_ptr, 
                             _IN_ const void* b_ptr,
                             _IN_ uint32_t size) noexcept;


        Memory_Manipulation() noexcept  = default;
        ~Memory_Manipulation() noexcept = default;
    };
} // namespace stdlib
