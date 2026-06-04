/* 
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:

NOTES:

*/

#pragma once

#include <stdint.h>

namespace shell::commands
{
    template <typename Arr>
    inline constexpr bool append_char(Arr& buffer, 
                                      uint32_t& index,
                                      const char symbol) noexcept {
        if (index < buffer.size()) [[likely]] {
            buffer[index++] = symbol;
            return true;
        }
        return false;
    }

    inline void print_command_error(const char* error_message) noexcept {
        runtime::text_output.set_text_color(
            drivers::vga::VGA_Textmode_Colors::YELLOW,
            drivers::vga::VGA_Textmode_Colors::BLACK
        );

        runtime::text_output.put_string(error_message);

        runtime::text_output.set_text_color(
            drivers::vga::VGA_Textmode_Colors::LIGHT_GREY,
            drivers::vga::VGA_Textmode_Colors::BLACK
        );
    }
} // namespace shell::commands
