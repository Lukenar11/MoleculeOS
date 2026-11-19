/**
 * @brief Implements a library for basic string manipulation.
 *
 * @copyright Copyright (c) 2026 Lukenar11 (Luke Matthes)
 * @license:  https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
 * @author    Lukenar11 (Luke Matthes)
 *
 * @note
 */


#include "string.hpp"


namespace
{
    using namespace stdlib;
    using namespace string;


    /**
     * @brief Validates the parameters for the most class methods.
     * 
     * @param destination pointer to validate
     * @param source      pointer to validate
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *          If `destination` is `nullptr`
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_B`
     *          If `source` is `nullptr` or empty.
     * 
     * @retval `status::SUCCESS`
     *          If all pointer are valid.
     */
    [[nodiscard]]
    status_t
    validate_destination_and_source(_IN_ const char* const destination,
                                    _IN_ const char* const source) noexcept {
        status_t status = status::SUCCESS;

        if (!destination) [[unlikely]] {
            status = status::NULL_POINTER | status::flags::PARAM_A;
            goto cleanup;
        }

        if (!source) [[unlikely]] {
            status = status::NULL_POINTER | status::flags::PARAM_B;
            goto cleanup;
        }
        
        if (source[0] == '\0') [[unlikely]] {
            status = status::NULL_POINTER | status::flags::PARAM_B;
        }

    cleanup:
        return status;
    } 
} // namespace


namespace stdlib::string
{
    /**
     * @brief Copys the content of a string part in another string.
     * 
     * @note The destination buffer must be at least `byte_size` bytes long.
     * 
     * @param destination pointer to destination string
     * @param source      pointer to source string
     * @param byte_size   string part char size
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *          If `destination` is `nullptr`.
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_B`
     *          If `source` is `nullptr` or empty.
     * 
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_C`
     *          If `byte_size` is `0` or `1`.
     * 
     * @retval `status::BUFFER_OVERFLOW`
     *          If `source` does not fit into the destination buffer.
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_ 
    [[nodiscard]] 
    status_t
    copy_part(_IN_ char* const destination,
              _IN_ const char* const source,
              _IN_ const uint32_t byte_size) noexcept {
        status_t status = status::SUCCESS;
        uint32_t index  = 0;

        status = validate_destination_and_source(destination, source);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        if (destination == source) [[unlikely]] {
            status = status::SUCCESS;
            goto cleanup;
        }

        if (byte_size <= 1) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_C;
            goto cleanup;
        }

        while (index + 1 < byte_size && source[index] != '\0') [[likely]] {
            destination[index] = source[index];
            index++;
        }

        if (source[index] != '\0') [[unlikely]] {
            status = status::BUFFER_OVERFLOW;
            goto cleanup;
        }

        destination[index] = '\0';

    cleanup:
        return status;
    }


    /**
     * @brief Copys the content of a string in another string.
     * 
     * @note The destination buffer must be large enough for the full string.
     * 
     * @param destination pointer to destination string
     * @param source      pointer to source string
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *          If `destination` is `nullptr`.
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_B`
     *          If `source` is `nullptr` or empty.
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_  
    [[nodiscard]]
    status_t 
    copy(_IN_ char* const destination, 
         _IN_ const char* const source) noexcept {
        status_t status = status::SUCCESS;
        uint32_t index  = 0;

        status = validate_destination_and_source(destination, source);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        if (destination == source) [[unlikely]] {
            status = status::SUCCESS;
            goto cleanup;
        }

        while (source[index] != '\0') [[likely]] {
            destination[index] = source[index];
            index++;
        }

        destination[index] = '\0';

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
    _API_ 
    [[nodiscard]] 
    status_t 
    find_char(_OUT_ const char*& found_char,
              _IN_  const char* const string, 
              _IN_  const int32_t symbol) noexcept {
        status_t status = status::NOT_FOUND;

        found_char = nullptr;

        if (!string) [[unlikely]] {
            status = status::NULL_POINTER | status::flags::PARAM_B;
            goto cleanup;
        }

        for (uint32_t index = 0; string[index] != '\0'; index++) [[likely]] {
            if (string[index] == static_cast<char>(symbol)) {
                found_char = &string[index];
                status     = status::SUCCESS;

                break;
            }
        }

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
     *          If `string` is a `nullptr` or empty.
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_  
    [[nodiscard]]
    status_t 
    get_length(_OUT_ uint32_t& length,
               _IN_  const char* const string) noexcept {
        status_t status = status::SUCCESS;

        length = 0;

        if (!string || string[0] == '\0') [[unlikely]] {
            status = status::NULL_POINTER | status::flags::PARAM_B;
            goto cleanup;
        }

        while (string[length] != '\0') [[likely]] {
            length++;
        }

        status = status::SUCCESS;

    cleanup:
        return status;
    }


    /**
     * @brief Compares 2 different strings.
     * 
     * @param string_a pointer to string a.
     * @param string_b pointer to string b.
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *          If `string_a` is `nullptr`.
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_B`
     *          If `string_b` is `nullptr`.
     * 
     * @retval `status::LESS_THAN` 
     *          If `string_a` is less then `string_b`.
     * 
     * @retval `status::GREATER_THAN` 
     *          If `string_a` is greater then `string_b`.
     * 
     * @retval `status::EQUAL_TO`
     *          If `string_a` and `string_b` are identical.
     */
    _API_  
    [[nodiscard]]
    status_t 
    compare(_IN_ const char* const string_a, 
            _IN_ const char* const string_b) noexcept {
        status_t status = status::EQUAL_TO;
        uint32_t index  = 0;

        if (!string_a) [[unlikely]] {
            status = status::NULL_POINTER | status::flags::PARAM_A;
            goto cleanup;
        }

        if (!string_b) [[unlikely]] {
            status = status::NULL_POINTER | status::flags::PARAM_B;
            goto cleanup;
        }

        if (string_a == string_b) [[unlikely]] {
            status = status::EQUAL_TO;
            goto cleanup;
        }

        while (string_a[index] != '\0' && 
               string_a[index] == string_b[index]) [[likely]] {
            index++;
        }

        if (string_a[index] < string_b[index]) {
            status = status::LESS_THAN;
        } 
        else if (string_a[index] > string_b[index]) {
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
     *          If the length of `string` is empty.
     * 
     * @retval `status::BUFFER_OVERFLOW`
     *          If `string` triggers an overflow.
     * 
     * @retval `status::BUFFER_UNDERFLOW`
     *          If `string` triggers an underflow.
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_
    [[nodiscard]]
    status_t
    to_int(_OUT_ int32_t& value,
           _IN_  const char* string) noexcept {
        status_t status              = status::SUCCESS;
        uint32_t string_length       = 0;
        bool is_negative             = false;
        uint32_t limit               = 0;
        uint32_t digit               = 0;
        uint32_t magnitude           = 0;
        const uint32_t ten           = 10;
        const uint32_t ascii_0       = '0';
        const uint32_t int32_min_abs = INT32_MAX + 1u;

        value = 0;

        if (!string) [[unlikely]] {
            status = status::NULL_POINTER | status::flags::PARAM_B;
            goto cleanup;
        }

        status = get_length(string_length, string);
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

            limit = int32_min_abs;
        }
        else {
            is_negative = false;
            limit       = INT32_MAX;
        }

        for (uint32_t index = 0; string[index] != '\0'; index++) [[likely]] {
            if (!is_digit(string[index])) [[unlikely]] {
                status = status::INVALID_PARAMETER | status::flags::PARAM_B;
                goto cleanup;
            }

            digit = static_cast<uint32_t>(string[index] - ascii_0);

            if (magnitude > limit / ten ||
                (magnitude == limit / ten && digit > limit % ten)) [[unlikely]] {
                if (is_negative) {
                    status = status::BUFFER_UNDERFLOW;
                }
                else {
                    status = status::BUFFER_OVERFLOW;
                }

                goto cleanup;
            }

            magnitude = magnitude * ten + digit;
        }

        if (is_negative) {
            if (magnitude == int32_min_abs) {
                value = INT32_MIN;
            }
            else {
                value = -static_cast<int32_t>(magnitude);
            }
        }
        else {
            value = static_cast<int32_t>(magnitude);
        }

    cleanup:
        return status;
    }


    /**
     * @brief Shows if a number is a digit.
     * 
     * @param symbol the symbol to check
     * 
     * @retval `true` if `symbol` is a digit.
     * @retval `false` if `symbol` is not a digit.
     */
    _API_ 
    [[nodiscard]] 
    bool 
    is_digit(_IN_ const int32_t symbol) noexcept {
        return symbol >= '0' && symbol <= '9';
    }
} // namespace stdlib::string
