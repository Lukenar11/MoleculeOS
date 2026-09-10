/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    Under the MIT License
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This library defines basic routines for memory manipulation
    that can be used sys-wide.

    The routines include copying, moving, comparing, and filling memory

NOTES:
*/


#include <memory_manip.hpp>


namespace stdlib
{
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
     * 
     * @retval `status::SUCCESS`
     *          All parameters are valid.
     */
    [[nodiscard]] 
    status_t
    Memory_Manipulation::validate_parameters(_IN_ void* destination_ptr, 
                                             _IN_ const void* source_ptr)
                                             noexcept {
        status_t status;

        if (!destination_ptr) [[unlikely]] {
            status = status::NULL_POINTER | status::flags::PARAM_A;
            goto cleanup;
        }

        if (!source_ptr) [[unlikely]] {
            status = status::NULL_POINTER | status::flags::PARAM_B;
            goto cleanup;
        }

        status = status::SUCCESS;

    cleanup:
        return status;
    }


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
     * @retval `status::SUCCESS | status::flags::byte_size_ZERO`
     *          If the memory-block-byte byte_size is `0`.
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_ 
    status_t 
    Memory_Manipulation::copy_memory_block(_IN_ void* destination_ptr, 
                                           _IN_ const void* source_ptr, 
                                           _IN_ uint32_t byte_size) noexcept {
        status_t status;
        uint8_t* destination_byte_ptr;
        const uint8_t* source_byte_ptr;

        status = validate_parameters(destination_ptr, source_ptr);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        if (byte_size == 0) [[unlikely]] {
            status = status::SUCCESS;
            goto cleanup;
        }

        if (destination_ptr == source_ptr) [[unlikely]] {
            status = status::SUCCESS;
            goto cleanup;
        }

        destination_byte_ptr = static_cast<uint8_t*>(destination_ptr);
        source_byte_ptr      = static_cast<const uint8_t*>(source_ptr);

        while (byte_size--) [[likely]] {
            *destination_byte_ptr++ = *source_byte_ptr++;
        }

        status = status::SUCCESS;

    cleanup:
        return status;
    }


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
     * @retval `status::SUCCESS | status::flags::byte_size_ZERO`
     *          If the memory-block-byte byte_size is `0`.
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_ 
    status_t 
    Memory_Manipulation::move_memory_block(_IN_ void* destination_ptr, 
                                           _IN_ const void* source_ptr, 
                                           _IN_ uint32_t byte_size) noexcept {
        status_t status;
        uint8_t* destination_byte_ptr;
        const uint8_t* source_byte_ptr;

        status = validate_parameters(destination_ptr, source_ptr);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        if (byte_size == 0) [[unlikely]] {
            status = status::SUCCESS;
            goto cleanup;
        }

        if (destination_ptr == source_ptr) [[unlikely]] {
            status = status::SUCCESS;
            goto cleanup;
        }
        
        destination_byte_ptr = static_cast<uint8_t*>(destination_ptr);
        source_byte_ptr  = static_cast<const uint8_t*>(source_ptr);

        if (reinterpret_cast<uint32_t>(destination_byte_ptr) < 
            reinterpret_cast<uint32_t>(source_byte_ptr) ||
            reinterpret_cast<uint32_t>(destination_byte_ptr) >= 
            reinterpret_cast<uint32_t>(source_byte_ptr) + 
            byte_size) [[likely]] {
            while (byte_size--) [[likely]] {
                *destination_byte_ptr++ = *source_byte_ptr++;
            }
        } 
        else {
            destination_byte_ptr += byte_size;
            source_byte_ptr      += byte_size;

            while (byte_size--) [[likely]] {
                *--destination_byte_ptr = *--source_byte_ptr;
            }
        }

        status = status::SUCCESS;

    cleanup:
        return status;
    }


    /**
     * @brief Fils a memory-block with a specific byte_size with a specific value.
     *
     * @param destination_ptr pointer to the memory-block
     * @param value           value with which the memory-block has to be filed
     * @param byte_size       memory-block byte byte_size
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *          If `destination_ptr` is a `nullptr`.

     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_ 
    status_t 
    Memory_Manipulation::set_memory_block(_IN_ void* destination_ptr, 
                                          _IN_ const int32_t value, 
                                          _IN_ uint32_t byte_size) noexcept {
        status_t status;
        uint8_t* destination_byte_ptr;
        uint8_t byte;

        if (!destination_ptr) [[unlikely]] {
            status = status::NULL_POINTER | status::flags::PARAM_A;
            goto cleanup;
        }

        if (byte_size == 0) [[unlikely]] {
            status = status::SUCCESS;
            goto cleanup;
        }

        destination_byte_ptr = static_cast<uint8_t*>(destination_ptr);
        byte                 = static_cast<uint8_t>(value);
        
        while (byte_size--) [[likely]] {
            *destination_byte_ptr++ = byte;
        }

        status = status::SUCCESS;

    cleanup:
        return status;
    };


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
     * @retval `status::SUCCESS | status::flags::byte_size_ZERO`
     *          If the memory-block-byte `byte_size` is `0`.
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
    Memory_Manipulation::compare_memory_block(_IN_ const void* a_ptr, 
                                              _IN_ const void* b_ptr,
                                              _IN_ uint32_t byte_size) 
                                              noexcept {
        status_t status;
        const uint8_t* a_byte_ptr = static_cast<const uint8_t*>(a_ptr);
        const uint8_t* b_byte_ptr = static_cast<const uint8_t*>(b_ptr);

        status = validate_parameters(const_cast<void*>(a_ptr), b_ptr);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        if (byte_size == 0) [[unlikely]] {
            status = status::SUCCESS;
            goto cleanup;
        }

        if (a_ptr == b_ptr) [[unlikely]] {
            status = status::EQUAL_TO;
            goto cleanup;
        }

        while (byte_size--) [[likely]] {
            if (*a_byte_ptr < *b_byte_ptr) {
                status = status::LESS_THAN;
                goto cleanup;  
            }

            if (*a_byte_ptr > *b_byte_ptr) {
                status = status::GREATER_THAN;
                goto cleanup;  
            }

            a_byte_ptr++;
            b_byte_ptr++;
        }

        status = status::EQUAL_TO;

    cleanup:
        return status;
    }
} // namespace stdlib
