/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This is a 'PS/2' keyboard input driver, 
    for reading translated keypresses from the keyboard controller (i8042).

    The driver reads raw scancodes from the data port, applies modifier
    state such as 'Shift' and 'CapsLock', and returns the resulting ASCII
    character to other components like the shell.

NOTES:
    Some methods are placed in the header 
    because they are so small that the compiler can inline them.
*/


#include "keyboard_input.hpp"
#include <io.hpp>


namespace drivers::ps2
{
    const char Keyboard_Input::qwerty_std_key_mapping[128] = {
        0, 27, '1','2','3','4','5','6','7','8','9','0', '-','=', '\b',
        '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
        'a','s','d','f','g','h','j','k','l',';','\'','`', 0, '\\',
        'z','x','c','v','b','n','m',',','.','/', 0, '*', 0, ' ',
    };

    
    const char Keyboard_Input::qwerty_shift_key_mapping[128] = {
        0, 27, '!','@','#','$','%','^','&','*','(',')','_','+', '\b',
        '\t','Q','W','E','R','T','Y','U','I','O','P','{','}','\n', 0,
        'A','S','D','F','G','H','J','K','L',':','\"','~', 0, '|',
        'Z','X','C','V','B','N','M','<','>','?', 0, '*', 0, ' ',
    };


    /** 
     * @brief Checks whether the 
     *        keyboard has a pressed but not handled scancode.
     * 
     * @retval `true`  Keyboard has a pending scancode.
     * @retval `false` Keyboard has not a pending scancode.
     */
    [[nodiscard]]
    bool 
    Keyboard_Input::has_pending_scancode() noexcept {
        return stdlib::byte_input(KEYBOARD_STATUS_PORT) & LOWEST_BIT;
    }


    /**
     * @brief Reads and gets keyboard key activity.
     * 
     * @param key pressed keyboard key
     * 
     * @retval `status::FAIL`
     *          If the keyboard key activity read fails.
     * 
     * @retval `status::PS2_SHIFT | status::flags::KEY_PRESS`
     *          If `SHIFT` was pressed.
     * 
     * @retval `status::PS2_SHIFT | status::flags::KEY_RELEASE`
     *          If `SHIFT` was released.
     * 
     * @retval `status::PS2_CAPS_LOCK`
     *          If `CAPS` was pressed.
     * 
     * @retval `status::PS2_KEYBOARD_RELEASE`
     *          Special fallback case if it can't return `status::SUCCESS` and 
     *          all other status code return cases are not `true`.
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     */
    [[nodiscard]] 
    status_t 
    Keyboard_Input::get_key(_OUT_ char& key) noexcept {
        status_t status;
        uint8_t scancode;

        if (!has_pending_scancode()) [[unlikely]] {
            status = status::FAIL;
            goto cleanup;
        }
    
        scancode = stdlib::byte_input(KEYBOARD_DATA_PORT);
    
        switch (scancode) {
        case static_cast<uint8_t>(Special_Keyboard_Keys::LEFT_SHIFT):
        case static_cast<uint8_t>(Special_Keyboard_Keys::RIGHT_SHIFT):
            shift_is_pressed = true;

            status = status::PS2_SHIFT | status::flags::KEY_PRESS;
            goto cleanup;
        
        case static_cast<uint8_t>(Special_Keyboard_Keys::LEFT_SHIFT_RELEASE):
        case static_cast<uint8_t>(Special_Keyboard_Keys::RIGHT_SHIFT_RELEASE):
            shift_is_pressed = false;

            status = status::PS2_SHIFT | status::flags::KEY_RELEASE;
            goto cleanup;
        
        case static_cast<uint8_t>(Special_Keyboard_Keys::CAPS):
            caps_is_enabled = !caps_is_enabled;

            status = status::PS2_CAPS_LOCK;
            goto cleanup;
        }
    
        if (scancode & 
            static_cast<uint8_t>(Special_Keyboard_Keys::KEYBOARD_RELEASE)) {
            status = status::PS2_KEYBOARD_RELEASE;
            goto cleanup;
        }
    
        if (scancode >= ALLOWED_SCANCODE_SIZE) [[unlikely]] {
            status = status::FAIL;
            goto cleanup;
        }
    
        if (!shift_is_pressed) [[likely]] {
            key = qwerty_std_key_mapping[scancode]; 
        }
        else [[unlikely]] {
            key = qwerty_shift_key_mapping[scancode];
        }

        if (caps_is_enabled) [[unlikely]] {
            if (key >= 'a' && key <= 'z') {
                key -= ASCII_CASE_OFFSET;
            }
            else if (key >= 'A' && key <= 'Z') {
                key += ASCII_CASE_OFFSET;
            }
        }

        status = status::SUCCESS;

        goto success;

    cleanup:
        key = '\0';

    success:
        return status;
    }


    /**
     * @brief Removes the entered keyboard key from the buffer.
     *
     * @param item The removed element.
     * 
     * @retval `status::BUFFER_UNDERFLOW | status::IS_EMPTY`
     *          If the buffer is empty.
     *
     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_
    [[nodiscard]]
    status_t
    Keyboard_Input::pop_scancode_key(_OUT_ char& item) noexcept {
        return scancode_buffer.pop(item);
    }

    
    /** 
     * @brief Pushed the pressed keyboard key into the scancode_buffer,
     *        if a  the interrupt event was triggered.
     * 
     * @note Do not use this in the normal code because 
     *       this method is only for the interrupts handling system.
     * 
     * @param register_dump dump of all CPU registers.
     */
    _API_ 
    void 
    Keyboard_Input::handle_keypress(_IN_ kernel::Registers* register_dump) noexcept {
        if (!register_dump) [[unlikely]] {
            goto cleanup;
        }

        if (!has_pending_scancode()) [[unlikely]] {
            goto cleanup;
        }

        char symbol;
        if (get_key(symbol) == status::SUCCESS) {
            scancode_buffer.push(symbol);
        }

    cleanup:
        return;
    }
} // namespace drivers::ps2
