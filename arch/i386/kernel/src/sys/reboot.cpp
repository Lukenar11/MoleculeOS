/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This is the kernel reboot routine, which triggers a
    hardware reset by issuing the 
    appropriate command to the keyboard controller (i8042).

NOTES:
*/

#include "sys/reboot.hpp"

namespace kernel::sys
{
    void reboot() noexcept {
        using enum drivers::ps2::Special_Keyboard_Keys;

        const uint8_t keyboard_ctrl     = static_cast<uint8_t>(CTRL);
        const uint8_t reboot_command    = static_cast<uint8_t>(REBOOT_COMMAND);
        const uint8_t input_buffer_full = 0x02;
        const uint8_t zero              = 0;

        if (!storemgr::Storage_Manager::save_filesys())
            panic("save failed");

        uint32_t timeout = 100'000;
        while (timeout--) {
            const uint8_t status = stdlib::byte_input(keyboard_ctrl);
            if ((status & input_buffer_full) == zero)
                break;
        }

        stdlib::byte_output(keyboard_ctrl, reboot_command);

        panic("Reboot failed");
    }
} // namespace kernel::sys
