/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    Under the MIT License
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This library defines basic routines for string manipulation
    that can be used system-wide.

    The routines include copying, shifting, comparing, and filling strings

NOTES:
    Some methods are defined in the header so that the 
    compiler can better inline them.
*/


#pragma once

#include <status.hpp>
#include <sal.hpp>
#include <types.hpp>


namespace stdlib
{
    class String_Manipulation final {
    private:
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
        [[nodiscard]] static status_t
        validate_dest_ptr_and_src_ptr(_INOUT_ char* dest_ptr,
                                      _IN_    const char* src_ptr) noexcept;

    public:
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
        _API_ static status_t 
        copy_string_part(_INOUT_ char* dest_ptr,
                         _IN_    const char* src_ptr,
                         _IN_    const uint32_t size) noexcept;


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
        _API_ static status_t 
        copy_string(_INOUT_ char* dest_ptr, 
                    _IN_    const char* src_ptr) noexcept;


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
        _API_ static status_t 
        find_char_in_string(_OUT_ const char*& founded_char,
                            _IN_  const char* string, 
                            _IN_  const int32_t symbol) noexcept;


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
        _API_ static status_t 
        get_string_length(_OUT_ uint32_t& length,
                          _IN_  const char *string) noexcept;


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
        _API_ static status_t 
        compare_strings(_IN_ const char* a_ptr, 
                        _IN_ const char* b_ptr) noexcept;


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
        _API_ static status_t 
        string_to_int(_OUT_ int32_t& value,
                      _IN_  const char* string) noexcept;


        /**
         * @brief Shows if a number is a digit.
         * 
         * @param symbol the symbol to check
         * 
         * @retval `true` if `symbol` is a digit.
         * @retval `false` if `symbol` is not a digit.
         */
        _API_ static bool 
        is_digit(_IN_ const int32_t symbol) noexcept {
            return (symbol >= '0') && (symbol <= '9');
        }


        String_Manipulation() noexcept  = default;
        ~String_Manipulation() noexcept = default;
    };
} // namespace stdlib
