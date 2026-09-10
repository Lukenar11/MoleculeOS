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
         * @param destination_ptr pointer to the validate
         * @param source_ptr      pointer to the validate
         * @param byte_size       byte byte_size to validate
         * 
         * @retval `status::NULL_POINTER | status::flags::PARAM_A`
         *          If `destination_ptr` is `nullptr`.
         * 
         * @retval `status::NULL_POINTER | status::flags::PARAM_B`
         *          If `source_ptr` is `nullptr`.

         * @retval `status::SUCCESS`
         *          All parameters are valid.
         */
        [[nodiscard]] 
        static 
        status_t
        validate_parameters(_IN_ void* destination_ptr, 
                            _IN_ const void* source_ptr) noexcept;

                            
    public:
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
        static 
        status_t 
        copy_memory_block(_IN_ void* destination_ptr, 
                          _IN_ const void* source_ptr, 
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
        static 
        status_t 
        move_memory_block(_IN_ void* destination_ptr, 
                          _IN_ const void* source_ptr, 
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
        static 
        status_t 
        set_memory_block(_IN_ void* destination_ptr, 
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
        static 
        status_t 
        compare_memory_block(_IN_ const void* a_ptr, 
                             _IN_ const void* b_ptr,
                             _IN_ uint32_t byte_size) noexcept;


        Memory_Manipulation() noexcept  = default;
        ~Memory_Manipulation() noexcept = default;
    };
} // namespace stdlib
