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

namespace kernel {
    struct Registers;
}

namespace drivers::ps2 
{
    class Keyboard_Input final {
    private:
        static inline constexpr uint8_t LOWEST_BIT            = 0x01;
        static inline constexpr uint8_t ALLOWED_SCANCODE_SIZE = 128;

        static inline constexpr uint16_t KEYBOARD_STATUS_PORT = 0x64;
        static inline constexpr uint16_t KEYBOARD_DATA_PORT   = 0x60;
    
        static inline bool shift_is_pressed = false;
        static inline bool caps_is_enabled  = false;

        static inline 
        bool has_pending_scancode() noexcept {
            return stdlib::byte_input(KEYBOARD_STATUS_PORT) & LOWEST_BIT;
        }

        [[nodiscard]] static 
        status_t get_key(_IN_ char& key) noexcept;

    public:
        static inline stdlib::Circular_Buffer<char, 
                                              ALLOWED_SCANCODE_SIZE> scancode_buffer;
        
        static 
        void keyboard_irq_handler(_OUT_ kernel::Registers*) noexcept;
    
        Keyboard_Input() noexcept  = default;
        ~Keyboard_Input() noexcept = default;
    };
} // namespace drivers::ps2
