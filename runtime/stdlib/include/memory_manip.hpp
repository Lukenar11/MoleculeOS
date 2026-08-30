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
        [[nodiscard]] static inline status_t
        validate_parameters(_IN_ void* dest_ptr, 
                            _IN_ const void* src_ptr, 
                            _IN_ uint32_t size) noexcept {
            status_t status;

            if (!dest_ptr) [[unlikely]] {
                status = status::NULL_POINTER | status::flags::PARAM_A;
                goto cleanup;
            }

            if (!src_ptr) [[unlikely]] {
                status = status::NULL_POINTER | status::flags::PARAM_B;
                goto cleanup;
            }

            if (size == 0) [[unlikely]] {
                status = status::SUCCESS | status::flags::SIZE_ZERO;
                goto cleanup;
            }

            status = status::SUCCESS;

        cleanup:
            return status;
        }


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
        _API_ static inline status_t 
        copy_memory_block(_IN_ void* dest_ptr, 
                          _IN_ const void* src_ptr, 
                          _IN_ uint32_t size) noexcept {
            status_t status;
            uint8_t* dest;
            const uint8_t* src;

            status = validate_parameters(dest_ptr, src_ptr, size);
            if (status != status::SUCCESS) [[unlikely]] {
                goto cleanup;
            }

            dest = static_cast<uint8_t*>(dest_ptr);
            src  = static_cast<const uint8_t*>(src_ptr);

            while (size--) [[likely]] {
                *dest++ = *src++;
            }

            status = status::SUCCESS;

        cleanup:
            return status;
        }


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
        _API_ static inline status_t 
        move_memory_block(_IN_ void* dest_ptr, 
                          _IN_ const void* src_ptr, 
                          _IN_ uint32_t size) noexcept {
            status_t status;
            uint8_t* dest;
            const uint8_t* src;

            status = validate_parameters(dest_ptr, src_ptr, size);
            if (status != status::SUCCESS) [[unlikely]] {
                goto cleanup;
            }
            
            dest = static_cast<uint8_t*>(dest_ptr);
            src  = static_cast<const uint8_t*>(src_ptr);

            if (reinterpret_cast<uint32_t>(dest) < 
                reinterpret_cast<uint32_t>(src)) [[likely]] {
                while (size--) [[likely]] {
                    *dest++ = *src++;
                }
            } 
            else {
                dest += size;
                src  += size;

                while (size--) [[likely]] {
                    *--dest = *--src;
                }
            }

            status = status::SUCCESS;

        cleanup:
            return status;
        }


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
        _API_ static inline status_t 
        set_memory_block(_IN_ void* dest_ptr, 
                         _IN_ const int32_t value, 
                         _IN_ uint32_t size) noexcept {
            status_t status;
            uint8_t* dest;
            uint8_t byte;

            if (!dest_ptr) [[unlikely]] {
                status = status::NULL_POINTER | status::flags::PARAM_A;
                goto cleanup;
            }

            if (size == 0) [[unlikely]] {
                status = status::SUCCESS | status::flags::SIZE_ZERO;
                goto cleanup;
            }

            dest = static_cast<uint8_t*>(dest_ptr);
            byte = static_cast<uint8_t>(value);
            
            while (size--) [[likely]] {
                *dest++ = byte;
            }

            status = status::SUCCESS;

        cleanup:
            return status;
        };


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
        _API_ static inline status_t 
        compare_memory_block(_IN_ const void* a_ptr, 
                             _IN_ const void* b_ptr,
                             _IN_ uint32_t size) noexcept {
            status_t status;
            const uint8_t* a = static_cast<const uint8_t*>(a_ptr);
            const uint8_t* b = static_cast<const uint8_t*>(b_ptr);

            status = validate_parameters(const_cast<void*>(a_ptr), 
                                         b_ptr,
                                         size);
            if (status != status::SUCCESS) [[unlikely]] {
                goto cleanup;
            }

            while (size--) [[likely]] {
                if (*a < *b) {
                    status = status::LESS_THAN;
                    goto cleanup;  
                }

                if (*a > *b) {
                    status = status::GREATER_THAN;
                    goto cleanup;  
                }

                a++;
                b++;
            }

            status = status::EQUAL_TO;

        cleanup:
            return status;
        }


        Memory_Manipulation() noexcept  = default;
        ~Memory_Manipulation() noexcept = default;
    };
} // namespace stdlib
