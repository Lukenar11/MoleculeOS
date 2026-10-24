/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file contains the high-level text output system, 
    which provides formatted and unformatted print functions 
    on the 'VGA text mode driver'.

    It provides character output, string output, 
    integer formatting, hexadecimal and binary print output, 
    pointer formatting and supports setting various text and cursor colors.

    The 'Text_Output' class manages its own cursor state, 
    color configuration, and line management, 
    allowing structured and readable output during runtime.
    
NOTES:
    Some methods are placed in the header 
    because they are so small that the compiler can inline them.
*/


#pragma once

#include <sal.hpp>
#include <types.hpp>
#include <status.hpp>


namespace drivers::vga
{
    enum class Text_Mode_Color : uint8_t;
    struct Text_Mode_Char;
} // namespace drivers::vga


namespace stdlib::console
{
    /** 
     * @brief Gets the X coordinate of the cursor
     * 
     * @return cursor X coordinate
     */        
    _API_ 
    [[nodiscard]] 
    uint32_t 
    get_cursor_x() noexcept;

    /** 
     * @brief Gets the Y coordinate of the cursor
     * 
     * @return cursor Y coordinate
     */   
    _API_ 
    [[nodiscard]] 
    uint32_t 
    get_cursor_y() noexcept;


    /** 
     * @brief Sets a new color for any other Terminal outputs.
     * 
     * @param background_color text background color (default black)
     * @param foreground_color text color (default light gray)
     */
    _API_ 
    void 
    set_text_color(_IN_ const drivers::vga::Text_Mode_Color background_color,
                   _IN_ const drivers::vga::Text_Mode_Color foreground_color)
                   noexcept;


    /** 
     * @brief Sets a new cursor position.
     * 
     * @param x X coordinate
     * @param y Y coordinate
     */
    _API_ 
    void
    set_cursor(_IN_ const uint32_t x, 
               _IN_ const uint32_t y) noexcept;


    /**
     * @brief Clears the screen and resets the cursor.
     */
    _API_  
    void 
    reset() noexcept;


    /** 
     * @brief Puts a signed integer.
     * 
     * @param value integer output
     *  
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_A` 
     *          If x is greater then or equal to the screen width.
     * 
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B` 
     *          If y is greater then or equal to the screen height.
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_A` 
     *          If the budder is a nullptr.
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_B` 
     *          If the buffer_count is a nullptr.
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_ 
    status_t 
    put_int(_IN_ int32_t value) noexcept;


    /** 
     * @brief Outputs a unsigned integer to the terminal.
     * 
     * @param value integer output
     * 
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_A` 
     *          If x is greater then or equal to the screen width.
     * 
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B` 
     *          If y is greater then or equal to the screen height.
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_A` 
     *          If the budder is a nullptr.
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_B` 
     *          If the buffer_count is a nullptr.
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_  
    status_t 
    put_uint(_IN_ const uint32_t value) noexcept;


    /** 
     * @brief Outputs a hexadecimal number to the terminal.
     * 
     * @retval `status::FAIL`
     *          Unknown error.
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     * 
     * @param value number output
     */
    _API_ 
    status_t
    put_hex(_IN_ const uint32_t value) noexcept;


    /** 
     * @brief Outputs a binary number to the terminal.
     * 
     * @retval `status::FAIL`
     *          Unknown error.
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     * 
     * @param value number output
     */
    _API_  
    status_t 
    put_bin(_IN_ const uint32_t value) noexcept;


    /** 
     * @brief Outputs the address of pointer to the 
     *        terminal in hexadecimal.
     * 
     * @retval `status::FAIL`
     *          Unknown error.
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     * 
     * @param value address output
     */
    _API_ 
    status_t 
    put_ptr(_IN_ const uint32_t value) noexcept;


    /** 
     * @brief Outputs a character to the terminal.
     * 
     * @param symbol character output
     * 
     * @note Allows the special characters:
     * @note - `\r` -> Cursor jump to the line begin.
     * @note - `\b` -> Delete the last character.
     * @note - `\t` -> Outputs a TAB.
     * @note - `\n` -> Cursor jump to the next line.
     * @note - `\\"` -> Allows `"` as character output.
     * 
     * @retval `status::FAIL`
     *          If the char puting fails.
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_  
    status_t 
    put_char(_IN_ const char symbol) noexcept;


    /** 
     * @brief Outputs a string to the terminal.
     * 
     * @param message string output
     * 
     * @note Allows the special characters:
     * @note - `\r` -> Cursor jump to the line begin.
     * @note - `\b` -> Delete the last character.
     * @note - `\t` -> Outputs a TAB.
     * @note - `\n` -> Cursor jump to the next line.
     * @note - `\\"` -> allows `"` in the string output
     * 
     * @retval `status::NULL_POINTER` 
     *          If `message` is a `nullptr`.
     * 
     * @retval `status::INVALID_PARAMETER`
     *          If `message` is empty.
     *
     * @retval `status::FAIL` 
     *          Unknown error.
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_  
    status_t
    put_string(_IN_ const char* const message) noexcept;
} // namespace stdlib::console
