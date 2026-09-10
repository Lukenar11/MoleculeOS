/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This library defines basic routines for string manipulation
    that can be used sys-wide.

    The routines include copying, shifting, comparing, and filling strings

NOTES:
    Some methods are defined in the header so that the 
    compiler can better inline them.
*/


#include <string_manip.hpp>


namespace stdlib
{
    /**
     * @brief Validates the parameters for the most class methods.
     * 
     * @param destination_ptr pointer to validate
     * @param source_ptr  pointer to validate
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *          If `destination_ptr` is `nullptr`.
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_B`
     *          If `source_ptr` is `nullptr`.
     * 
     * @retval `status::SUCCESS`
     *          If all pointer are valid.
     */
    [[nodiscard]] 
    status_t
    String_Manipulation::validate_destination_ptr_and_source_ptr(_INOUT_ char* destination_ptr,
                                                                 _IN_    const char* source_ptr)
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
     * @brief Copys the content of a string part in another string.
     * 
     * @note The destination buffer must be at least `byte_size` bytes long.
     * 
     * @param destination_ptr pointer to destination-string
     * @param source_ptr      pointer to source-string
     * @param byte_size       string part char size
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_A` 
     *          If `destination_ptr` is a `nullptr`.
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_B` 
     *          If `source_ptr` is a `nullptr`.
     * 
     * @retval `status::SUCCESS | status::flags::SIZE_ZERO`
     *          If `byte_size` is a `0` or `1`.
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_ 
    status_t
    String_Manipulation::copy_string_part(_INOUT_ char* destination_ptr,
                                          _IN_    const char* source_ptr,
                                          _IN_    const uint32_t byte_size) 
                                          noexcept {
        status_t status;
        uint32_t memory_index = 0;
        const char null_char  = '\0';

        status = validate_destination_ptr_and_source_ptr(destination_ptr, 
                                                         source_ptr);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        if (destination_ptr == source_ptr) [[unlikely]] {
            status = status::SUCCESS;
            goto cleanup;
        }

        if (byte_size <= 1) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_C;
            goto cleanup;
        }

        while (memory_index + 1 < 
               byte_size && source_ptr[memory_index] != null_char) [[likely]] {
            destination_ptr[memory_index] = source_ptr[memory_index];
            ++memory_index;
        }

        if (source_ptr[memory_index] != null_char) [[unlikely]] {
            status = status::BUFFER_OVERFLOW;
            goto cleanup;
        }

        destination_ptr[memory_index] = null_char;

        status = status::SUCCESS;

    cleanup:
        return status;
    }


    /**
     * @brief Copys the content of a string in another string.
     * 
     * @note The destination buffer must be large enough for the full string.
     * 
     * @param destination_ptr pointer to the destination-string
     * @param source_ptr      pointer to the source-string
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_A` 
     *          If `destination_ptr` is a `nullptr`.
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_B` 
     *          If `source_ptr` is a `nullptr`.
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_ status_t 
    String_Manipulation::copy_string(_INOUT_ char* destination_ptr, 
                                     _IN_    const char* source_ptr) noexcept {
        status_t status;

        status = validate_destination_ptr_and_source_ptr(destination_ptr, 
                                                         source_ptr);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        if (destination_ptr == source_ptr) [[unlikely]] {
            status = status::SUCCESS;
            goto cleanup;
        }

        while ((*destination_ptr++ = *source_ptr++)) [[likely]] {
        }

        status = status::SUCCESS;

    cleanup:
        return status;
    }


    /**
     * @brief Finds a specific symbol in a string and returns the position.
     * 
     * @param found_char the position of the found symbol
     * @param string     the string to search
     * @param symbol     the symbol to search for
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_B`
     *          If `string` is a `nullptr`
     * 
     * @retval `status::NOT_FOUND`
     *          If `symbol` was not found.
     * 
     * @retval `status::SUCCESS` 
     *          Default case.
     */
    _API_ status_t 
    String_Manipulation::find_char_in_string(_OUT_ const char*& found_char,
                                             _IN_  const char* string, 
                                             _IN_  const int32_t symbol) 
                                             noexcept {
        status_t status;

        if (!string) [[unlikely]] {
            found_char = nullptr;
            status     = status::NULL_POINTER | status::flags::PARAM_B;

            goto cleanup;
        }

        while (*string != static_cast<char>(symbol)) [[likely]] {
            if (!(*string++)) [[unlikely]] {
                found_char = nullptr;
                status     = status::NOT_FOUND;

                goto cleanup;
            }
        }

        found_char = const_cast<char*>(string);
        status     = status::SUCCESS;

    cleanup:
        return status;
    }


    /**
     * @brief Gets the length of a string.
     * 
     * @param length the final string length. 
     * @param string the string for the length calculation.
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_B`
     *          If `string` is a `nullptr`.
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_ status_t 
    String_Manipulation::get_string_length(_OUT_ uint32_t& length,
                                           _IN_  const char* string) noexcept {
        status_t status;
        length = 0;

        if (!string) [[unlikely]] {
            status = status::NULL_POINTER | status::flags::PARAM_B;
            goto cleanup;
        }

        while (*string != '\0') [[likely]] {
            ++string;
            ++length;
        }

        status = status::SUCCESS;

    cleanup:
        return status;
    }


    /**
     * @brief Compares 2 different strings.
     * 
     * @param a_ptr pointer to string-a.
     * @param b_ptr pointer to string-b.
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *          If `a_ptr` is `nullptr`.
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_B`
     *          If `b_ptr` is `nullptr`.
     * 
     * @retval `status::LESS_THAN` 
     *          If `string-a` is less then `string-b`.
     * 
     * @retval `status::GREATER_THAN` 
     *          If `string-a` is greater then `string-b`.
     * 
     * @retval `status::EQUAL_TO`
     *          If string-a and string-b are identical.
     */
    _API_ status_t 
    String_Manipulation::compare_strings(_IN_ const char* a_ptr, 
                                         _IN_ const char* b_ptr) noexcept {
        status_t status;

        if (!a_ptr) [[unlikely]] {
            status = status::NULL_POINTER | status::flags::PARAM_A;
            goto cleanup;
        }

        if (!b_ptr) [[unlikely]] {
            status = status::NULL_POINTER | status::flags::PARAM_B;
            goto cleanup;
        }

        if (a_ptr == b_ptr) [[unlikely]] {
            status = status::EQUAL_TO;
            goto cleanup;
        }

        while (*a_ptr && (*a_ptr == *b_ptr)) [[likely]] {
            ++a_ptr;
            ++b_ptr;
        }

        if (*a_ptr < *b_ptr) {
            status = status::LESS_THAN;
        } 
        else if (*a_ptr > *b_ptr) {
            status = status::GREATER_THAN;
        }
        else {
            status = status::EQUAL_TO;
        }

    cleanup:
        return status;
    }


    /**
     * @brief Converts a string to an negative or non negative integer.
     * 
     * @param value  converted value.
     * @param string string for converting.
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_B`
     *          If `string` is a `nullptr`.
     * 
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
     *          If one or more char/chars in `string` is not a value.
     * 
     * @retval `status::EMPTY`
     *          If the length of `string` is to short.
     * 
     * @retval `status::BUFFER_OVERFLOW`
     *          If `string` triggers an overflow.
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_ 
    status_t 
    String_Manipulation::string_to_int(_OUT_ int32_t& value,
                                       _IN_  const char* string) noexcept {
        bool is_negative;
        status_t status;
        uint32_t digit;
        uint32_t string_length = 0;
        uint32_t limit         = 0;
        uint32_t magnitude     = 0;
        const uint32_t ten     = 10;
        value                  = 0;

        if (!string) [[unlikely]] {
            status = status::NULL_POINTER | status::flags::PARAM_B;
            goto cleanup;
        }

        status = get_string_length(string_length, string);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        if (string_length == 0) [[unlikely]] {
            status = status::EMPTY;
            goto cleanup;
        }

        if (string[0] == '-') {
            is_negative = true;
            string++;

            if (string_length <= 1) [[unlikely]] {
                status = status::EMPTY;
                goto cleanup;
            }

            limit = INT32_MIN;
        }
        else {
            is_negative = false;
            limit       = INT32_MAX;
        }

        while (*string) {
            if (!is_digit(*string)) [[unlikely]] {
                status = status::INVALID_PARAMETER | status::flags::PARAM_B;
                goto cleanup;
            }

            digit = static_cast<uint32_t>(*string - '0');

            if  (magnitude > limit / ten ||
                 (digit > limit % ten &&
                  magnitude == limit / ten)) [[unlikely]] {
                status = (is_negative) 
                         ? status::BUFFER_UNDERFLOW      
                         : status::BUFFER_OVERFLOW;
                goto cleanup;
            }

            magnitude = magnitude * ten + digit;
            ++string;
        }

        value = (is_negative) 
                ? -static_cast<int32_t>(magnitude)
                :  static_cast<int32_t>(magnitude);

        status = status::SUCCESS;

    cleanup:
        return status;
    }
} // namespace stdlib
