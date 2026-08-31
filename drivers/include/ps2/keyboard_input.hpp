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


#pragma once

#include "../utils/ps2_keyboard_in_helpers.hpp"
#include <types.hpp>
#include <circular_buffer.hpp>
#include <io.hpp>
#include <status.hpp>
#include <sal.hpp>
#include <kernel.hpp>


namespace kernel 
{
    struct Registers;
}


namespace drivers::ps2 
{
    class Keyboard_Input final {
    private:
        static inline constexpr uint8_t LOWEST_BIT            = 0x01;
        static inline constexpr uint8_t ALLOWED_SCANCODE_SIZE = 128;
        static inline constexpr uint8_t ASCII_CASE_OFFSET     = 32;

        static inline constexpr uint16_t KEYBOARD_STATUS_PORT = 0x64;
        static inline constexpr uint16_t KEYBOARD_DATA_PORT   = 0x60;
    
        static inline bool shift_is_pressed = false;
        static inline bool caps_is_enabled  = false;

        
        /** 
         * @brief Checks whether the 
         *        keyboard has a pressed but not handled scancode.
         * 
         * @retval `true`  Keyboard has a pending scancode.
         * @retval `false` Keyboard has not a pending scancode.
         */
        static inline bool 
        has_pending_scancode() noexcept {
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
         *          Special fallback case if it can't return `status::SUCCESS` 
         *          and all other status code return cases are not `true`.
         * 
         * @retval `status::SUCCESS`
         *          Default case.
         */
        [[nodiscard]] static status_t 
        get_key(_OUT_ char& key) noexcept;


    public:
        _API_ static inline 
        stdlib::Circular_Buffer<char, ALLOWED_SCANCODE_SIZE> scancode_buffer;
        

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
        _API_ static void 
        keyboard_irq_handler(_IN_ kernel::Registers*) noexcept;
    

        Keyboard_Input() noexcept  = default;
        ~Keyboard_Input() noexcept = default;
    };
} // namespace drivers::ps2
