/* 
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

#include "kernel/include/filesystem/mofs.hpp"
#include "utils/append_char.hpp"
#include <text_output.hpp>
#include <stdint.h>
#include <array.hpp>
#include <string.h>

namespace shell::commands
{
    inline void print_command_error(const char* error_message) noexcept {
        runtime::text_output.set_text_color(
            drivers::vga::Text_Mode_Colors::YELLOW,
            drivers::vga::Text_Mode_Colors::BLACK
        );

        runtime::text_output.put_string(error_message);

        runtime::text_output.set_text_color(
            drivers::vga::Text_Mode_Colors::LIGHT_GREY,
            drivers::vga::Text_Mode_Colors::BLACK
        );
    }

    inline void command_end() noexcept {
        const char new_line = '\n';
        runtime::text_output.put_char(new_line);
        runtime::text_output.put_char(new_line);
    }

    struct Parsed_File_Name final {
        runtime::Array<char, 60> name;
        runtime::Array<char, 4> format;
        runtime::Array<char, 32> error;
    };

    Parsed_File_Name& parse_filename(const runtime::Array<char, 64>& arguments)
                                     noexcept;
} // namespace shell::commands
