/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file contains a few helper functions 
    that are used by the shell command implementations.

NOTES:
    Since some functions are quite small, they are intentionally placed in the
    header so that the compiler can inline them more easily.
*/

#pragma once

#include "utils.hpp"
#include <kernel.hpp>
#include <console.hpp>
#include <types.hpp>
#include <array.hpp>
#include <string.hpp>

namespace shell::commands
{
    struct Parsed_File_Name final {
        stdlib::Array<char, kernel::filesys::MAX_FILE_NAME_LENGTH + 1> name;
        stdlib::Array<char, kernel::filesys::MAX_FILE_FORMAT_LENGTH + 1> format;
        stdlib::Array<char, 32> error;
    };

    inline void print_command_error(const char* error_message) noexcept {
        stdlib::console::set_text_color(drivers::vga::Text_Mode_Color::BLACK,
                                        drivers::vga::Text_Mode_Color::YELLOW);

        stdlib::console::put_string(error_message);

        stdlib::console::set_text_color(drivers::vga::Text_Mode_Color::BLACK,
                                        drivers::vga::Text_Mode_Color::LIGHT_GREY);
    }

    inline void command_end() noexcept {
        stdlib::console::put_char('\n');
        stdlib::console::put_char('\n');
    }

    Parsed_File_Name& parse_filename(const stdlib::Array<char, 64>& args) noexcept;
} // namespace shell::commands
