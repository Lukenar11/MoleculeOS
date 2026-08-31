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


#include "../ps2/keyboard_input.hpp"


namespace drivers::ps2
{
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
            key = us_qwerty_std_key_mapping[scancode]; 
        }
        else [[unlikely]] {
            key = us_qwerty_shift_key_mapping[scancode];
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
     * @brief Pushed the pressed keyboard key into the scancode_buffer,
     *        if a  the interrupt descriptor event `IRQ 1` was triggered.
     * 
     * @note Do not use this in the normal code because 
     *       this method is only for the `IRQ`, 
     *       use the keyboard scancode_buffer instead.
     * 
     * @param Registers dump of all CPU registers (not used).
     */
    _API_ void 
    Keyboard_Input::keyboard_irq_handler(_IN_ kernel::Registers*) noexcept {
        char symbol;
        status_t status;  

        if (!has_pending_scancode()) [[unlikely]] {
            goto cleanup;
        }

        status = get_key(symbol);
        if (status == status::SUCCESS) {
            scancode_buffer.push(symbol);
        }

    cleanup:
        return;
    }
} // namespace drivers::ps2
