/**
 * @brief Defines a library for basic string manipulation.
 *
 * @copyright Copyright (c) 2026 Lukenar11 (Luke Matthes)
 * @license:  https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
 * @author    Lukenar11 (Luke Matthes)
 *
 * @note
 */


#pragma once

#include <sal.hpp>
#include <status.hpp>
#include <types.hpp>


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
    copy_part(_IN_ char* const destination_ptr,
              _IN_ const char* const source_ptr,
              _IN_ const uint32_t byte_size) noexcept;


    /**
     * @brief Copys the content of a string in another string.
     * 
     * @note The destination buffer must be large enough for the 
     *       full string.
     * 
     * @param destination_ptr pointer to destination string
     * @param source_ptr      pointer to source string
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
    copy(_IN_ char* const destination_ptr, 
         _IN_ const char* const source_ptr) noexcept;


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
              _IN_  const int32_t symbol) noexcept;


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
               _IN_  const char* const string) noexcept;


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
            _IN_ const char* const string_b) noexcept;


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
           _IN_  const char* string) noexcept;


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
    is_digit(_IN_ const int32_t symbol) noexcept;
} // namespace stdlib::string
