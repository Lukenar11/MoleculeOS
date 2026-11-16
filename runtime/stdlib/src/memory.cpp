/**
 * @brief Implements a library for basic memory manipulation.
 *
 * @copyright Copyright (c) 2026 Lukenar11 (Luke Matthes)
 * @license:  https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
 * @author    Lukenar11 (Luke Matthes)
 *
 * @note
 */


#include <memory.hpp>


namespace
{
    /**
     * @brief Validates the parameters for the most class methods.
     * 
     * @param destination pointer to the validate
     * @param source      pointer to the validate
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *          If `destination` is `nullptr`.
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_B`
     *          If `source` is `nullptr`.
     * 
     * @retval `status::SUCCESS`
     *          All parameters are valid.
     */
    [[nodiscard]] 
    status_t
    validate_destination_and_source(_IN_ const void* const destination, 
                                    _IN_ const void* const source) noexcept {
        status_t status = status::SUCCESS;

        if (!destination) [[unlikely]] {
            status = status::NULL_POINTER | status::flags::PARAM_A;
            goto cleanup;
        }

        if (!source) [[unlikely]] {
            status = status::NULL_POINTER | status::flags::PARAM_B;
        }

    cleanup:
        return status;
    }
} // namespace


namespace stdlib::memory
{
    /**
     * @brief Copys a memory block with a specific byte_size.
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
               _IN_ const uint32_t byte_size) noexcept {
        status_t status            = status::SUCCESS;
        uint8_t* destination_byte  = nullptr;
        const uint8_t* source_byte = nullptr;

        status = validate_destination_and_source(destination, source);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        if (byte_size == 0 || destination == source) [[unlikely]] {
            goto cleanup;
        }

        destination_byte = static_cast<uint8_t* const>(destination);
        source_byte      = static_cast<const uint8_t* const>(source);

        for (uint32_t byte_index = 0; 
             byte_index < byte_size; 
             byte_index++) [[likely]] {
            destination_byte[byte_index] = source_byte[byte_index];
        }

    cleanup:
        return status;
    }


    /**
     * @brief Copys a memory block with a specific byte_size and 
     *        alows backward copying.
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
    move_block(_IN_ void* const destination, 
               _IN_ const void* const source, 
               _IN_ const uint32_t byte_size) noexcept {
        status_t status            = status::SUCCESS;
        uint8_t* destination_byte  = nullptr;
        const uint8_t* source_byte = nullptr;

        status = validate_destination_and_source(destination, source);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        if (byte_size == 0) [[unlikely]] {
            status = status::SUCCESS;
            goto cleanup;
        }

        if (destination == source) [[unlikely]] {
            status = status::SUCCESS;
            goto cleanup;
        }
        
        destination_byte = static_cast<uint8_t* const>(destination);
        source_byte      = static_cast<const uint8_t* const>(source);

        if (reinterpret_cast<uint32_t>(destination_byte) < 
            reinterpret_cast<uint32_t>(source_byte) ||
            reinterpret_cast<uint32_t>(destination_byte) >= 
            reinterpret_cast<uint32_t>(source_byte) + 
            byte_size) [[likely]] {
            for (uint32_t byte_index = 0; 
                 byte_index < byte_size; 
                 byte_index++) [[likely]] {
                destination_byte[byte_index] = source_byte[byte_index];
            }
        } 
        else {
            destination_byte += byte_size;
            source_byte      += byte_size;
        
            for (uint32_t byte_index = byte_size; 
                 byte_index > 0; 
                 --byte_index) [[likely]] {
                destination_byte[byte_index - 1] = source_byte[byte_index - 1];
            } 
        }

    cleanup:
        return status;
    }


    /**
     * @brief Fils a memory block with a specific byte size 
     *        with a specific value.
     *
     * @param destination pointer to the memory block
     * @param value       value with which the memory block has to be filed
     * @param byte_size   memory block byte byte size
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *          If `destination` is a `nullptr`.

     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_ 
    [[nodiscard]]
    status_t 
    set_block(_IN_ void* const destination, 
              _IN_ const int32_t value, 
              _IN_ const uint32_t byte_size) noexcept {
        status_t status           = status::SUCCESS;
        uint8_t* destination_byte = nullptr;
        uint8_t byte              = 0;

        if (!destination) [[unlikely]] {
            status = status::NULL_POINTER | status::flags::PARAM_A;
            goto cleanup;
        }

        if (byte_size == 0) [[unlikely]] {
            status = status::SUCCESS;
            goto cleanup;
        }

        destination_byte = static_cast<uint8_t* const>(destination);
        byte             = static_cast<uint8_t>(value);
        
        for (uint32_t byte_index = 0; 
             byte_index < byte_size; 
             byte_index++) [[likely]] {
            destination_byte[byte_index] = byte;
        }

    cleanup:
        return status;
    };


    /**
     * @brief Compares the values in two diffenent memory blocks 
     *        with a specific byte size.
     *
     * @param a           pointer to destination memory block a
     * @param b           pointer to source memory block b
     * @param byte_size   memory block byte byte size
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
                  _IN_ uint32_t byte_size) noexcept {
        status_t status       = status::EQUAL_TO;
        const uint8_t* a_byte = nullptr;
        const uint8_t* b_byte = nullptr;

        status = validate_destination_and_source(a, b);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        if (byte_size == 0) [[unlikely]] {
            status = status::SUCCESS;
            goto cleanup;
        }

        if (a == b) [[unlikely]] {
            status = status::EQUAL_TO;
            goto cleanup;
        }

        a_byte = static_cast<const uint8_t* const>(a);
        b_byte = static_cast<const uint8_t* const>(b);

        while (byte_size--) [[likely]] {
            if (*a_byte < *b_byte) {
                status = status::LESS_THAN;
                goto cleanup;  
            }

            if (*a_byte > *b_byte) {
                status = status::GREATER_THAN;
                goto cleanup;  
            }

            a_byte++;
            b_byte++;
        }

        status = status::EQUAL_TO;

    cleanup:
        return status;
    }
} // namespace stdlib::memory
