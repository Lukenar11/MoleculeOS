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
#include <text_output.hpp>
#include <types.hpp>
#include <array.hpp>
#include <string_manip.hpp>

namespace shell::commands
{
    struct Parsed_File_Name final {
        runtime::Array<char, kernel::filesys::MAX_FILE_NAME_LENGTH + 1> name;
        runtime::Array<char, kernel::filesys::MAX_FILE_FORMAT_LENGTH + 1> format;
        runtime::Array<char, 32> error;
    };

    inline void print_command_error(const char* error_message) noexcept {
        runtime::Text_Output::set_text_color(drivers::vga::Text_Mode_Colors::YELLOW,
                                             drivers::vga::Text_Mode_Colors::BLACK);

        runtime::Text_Output::put_string(error_message);

        runtime::Text_Output::set_text_color(drivers::vga::Text_Mode_Colors::LIGHT_GREY,
                                             drivers::vga::Text_Mode_Colors::BLACK);
    }

    inline void command_end() noexcept {
        runtime::Text_Output::put_char('\n');
        runtime::Text_Output::put_char('\n');
    }

    Parsed_File_Name& parse_filename(const runtime::Array<char, 64>& args) noexcept;
} // namespace shell::commands
