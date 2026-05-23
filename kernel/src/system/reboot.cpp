/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This is the kernel reboot routine, which triggers a
    hardware reset by issuing the appropriate command to the keyboard controller (i8042).

NOTES:
*/

#include "system/reboot.hpp"

namespace kernel::system
{
    void reboot() noexcept {
        using enum drivers::ps2::KeyboardSpecialKeys;

        const uint8_t keyboard_ctrl = static_cast<uint8_t>(CTRL);
        const uint8_t rboot_command = static_cast<uint8_t>(REBOOT_COMMAND);
        const uint8_t input_buffer_full = 0x02;

        uint32_t timeout = 100'000;
        while (timeout--) {
            const uint8_t status = inb(keyboard_ctrl);
            if ((status & input_buffer_full) == NULL)
                break;
        }

        // set reboot
        outb(keyboard_ctrl, rboot_command);

        panic(
            "Reboot failed",
            "The keyboard controller did not trigger a hardware reset.\n"
            "This should never happen.\n"
            "Please report this to the developer."
        );
    }
} // namespace kernel::system
