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
     * @param dest_ptr pointer to validate
     * @param src_ptr  pointer to validate
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *          If `dest_ptr` is `nullptr`.
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_B`
     *          If `src_ptr` is `nullptr`.
     * 
     * @retval `status::SUCCESS`
     *          If all pointer are valid.
     */
    [[nodiscard]] status_t
    String_Manipulation::validate_dest_ptr_and_src_ptr(_INOUT_ char* dest_ptr,
                                                      _IN_ const char* src_ptr)
                                                       noexcept {
        status_t status;

        if (!dest_ptr) [[unlikely]] {
            status = status::NULL_POINTER | status::flags::PARAM_A;
            goto cleanup;
        }

        if (!src_ptr) [[unlikely]] {
            status = status::NULL_POINTER | status::flags::PARAM_B;
            goto cleanup;
        }

        status = status::SUCCESS;

    cleanup:
        return status;
    }


    /**
     * @brief Copys the content of a string part in another string.
     * @note The destination buffer must be at least `size` bytes long.
     * 
     * @param dest_ptr pointer to destination-string
     * @param src_ptr  pointer to source-string
     * @param size     string part char size
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_A` 
     *          If `dest_ptr` is a `nullptr`.
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_B` 
     *          If `src_ptr` is a nullptr`.
     * 
     * @retval `status::SUCCESS | status::flags::SIZE_ZERO`
     *          If `size` is a `0`.
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_ status_t 
    String_Manipulation::copy_string_part(_INOUT_ char* dest_ptr,
                                          _IN_    const char* src_ptr,
                                          _IN_    const uint32_t size) 
                                          noexcept {
        status_t status;
        const char null_char = '\0';

        status = validate_dest_ptr_and_src_ptr(dest_ptr, src_ptr);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        if (size == 0) [[unlikely]] {
            status = status::SUCCESS | status::flags::SIZE_ZERO;
            goto cleanup;
        }

        for (uint32_t i = 0; i < size; i++) [[likely]] {
            if (src_ptr[i] == null_char) {
                dest_ptr[i] = null_char;
                status = status::SUCCESS;
                goto cleanup;
            }

            dest_ptr[i] = src_ptr[i];
        }

        dest_ptr[size - 1] = null_char;
        status = status::SUCCESS;

    cleanup:
        return status;
    }


    /**
     * @brief Copys the content of a string in another string.
     * 
     * @param dest_ptr pointer to the destination-string
     * @param src_ptr  pointer to the source-string
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_A` 
     *          If `dest_ptr` is a `nullptr`.
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_B` 
     *          If `src_ptr` is a nullptr`.
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_ status_t 
    String_Manipulation::copy_string(_INOUT_ char* dest_ptr, 
                                     _IN_    const char* src_ptr) noexcept {
        status_t status;

        status = validate_dest_ptr_and_src_ptr(dest_ptr, src_ptr);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        while ((*dest_ptr++ = *src_ptr++)) [[likely]] {
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
    String_Manipulation::find_char_in_string(_OUT_ const char*& founded_char,
                                             _IN_  const char* string, 
                                             _IN_  const int32_t symbol) 
                                             noexcept {
        status_t status;

        if (!string) [[unlikely]] {
            founded_char = nullptr;
            status       = status::NULL_POINTER | status::flags::PARAM_B;

            goto cleanup;
        }

        while (*string != static_cast<char>(symbol)) [[likely]] {
            if (!(*string++)) [[unlikely]] {
                founded_char = nullptr;
                status       = status::NOT_FOUND;

                goto cleanup;
            }
        }

        founded_char = const_cast<char*>(string);
        status       = status::SUCCESS;

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
                                           _IN_  const char *string) noexcept {
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

        status = validate_dest_ptr_and_src_ptr(const_cast<char*>(a_ptr), 
                                               b_ptr);
        if (status != status::SUCCESS) [[unlikely]] {
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
     * @retval `status::INVALID_PARAMETER`
     *          If one or more char/chars in `string` is not a value.
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_ status_t 
    String_Manipulation::string_to_int(_OUT_ int32_t& value,
                                       _IN_  const char* string) noexcept {
        status_t status;
        bool is_negative;
        value = 0;

       if (!string) [[unlikely]] {
            status = status::NULL_POINTER | status::flags::PARAM_B;
            goto cleanup;
        }

        if (string[0] == '-') {
            is_negative = true;
            string++;
        }
        else {
            is_negative = false;
        }

        while (*string) [[likely]] {
            if (!is_digit(*string)) [[unlikely]] {
                status = status::INVALID_PARAMETER;
                goto cleanup;
            }

            value = value * 10 + (*string - '0');
            ++string;
        }

        if (is_negative) {
            value = ~value + 1;
        }

        status = status::SUCCESS;

    cleanup:
        return status;
    }
} // namespace stdlib
