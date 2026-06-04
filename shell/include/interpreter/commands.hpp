/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This defines the built-in shell commands that are available in
    MoleculeOS. Each command is implemented as a lightweight function
    that performs a specific action.

    These commands are called by the shell interpreter via a
    compile-time generated command list that maps hashed command strings 
    to their corresponding handlers.

NOTES:
    All these shell command implementations are intentionally 
    only implemented in the header, 
    as they are quite small, so that the compiler can inline them.
*/

#pragma once

#include "kernel/include/system/reboot.hpp"
#include "kernel/include/system/shutdown.hpp"
#include "kernel/include/filesystem/mofs.hpp"
#include <stdint.h>
#include <text_output.hpp>
#include <array.hpp>

namespace shell::commands
{
    inline void help() noexcept {
        runtime::text_output.reset();

        static const char help_message[] = {
            "Command syntax:\n"
            "\t- [Command]\n"
            "\t- [Command] [Argument]\n"
            "\nAvailable commands:\n"
            "\t- help (displays a command-line explanation) | help\n"
            "\t- info (displays the MoleculeOS startup screen) | info\n"
            "\t- clear (clears the screen) | clear\n"
            "\t- reboot (restart the Computer) | reboot\n"
            "\t- shutdown (turn the Computer off) | shutdown\n"
            "\t- echo (displays a message) | echo [Message]\n"
            "\t- create (creates a file) | create [FileName].[FileFormat]\n"
            "\n"
        };

        runtime::text_output.put_string(help_message);
    }

    inline void info() noexcept {
        runtime::text_output.reset();

        static const char info_message[] = {
            "__   __       _                 _         ____   _____ \n"
            "|  \\/  |     | |               | |       / __ \\ / ____|\n"
            "| \\  / | ___ | | ___  ___ _   _| | ___  | |  | | (___  \n"
            "| |\\/| |/ _ \\| |/ _ \\/ __| | | | |/ _ \\ | |  | |\\___ \\ \n"
            "| |  | | (_) | |  __/ (__| |_| | |  __/ | |__| |____) |\n"
            "|_|  |_|\\___/|_|\\___|\\___|\\__,_|_|\\___|  \\____/|_____/ \n"
            "\nVersion 0.1.0-alpha (Kernel Mode)\n"
            "\nCopyright (c) 2026 Lukenar11 (Luke Matthes)\n"
            "MIT Licensed\n"
            "https://github.com/Lukenar11/MoleculeOS\n"
            "\n"
        };

        runtime::text_output.put_string(info_message);
    }

    inline void clear() noexcept {
        runtime::text_output.reset();
    }

    inline void reboot() noexcept {
        kernel::system::reboot();
    }

    inline void shutdown() noexcept {
        kernel::system::shutdown();
    }

    inline void echo(const runtime::Array<char, 64>& arguments) noexcept {
        const char null_terminator = '\0';

        if (arguments[0] == null_terminator) [[unlikely]] {
            runtime::text_output.set_text_color(
                drivers::vga::VGA_Textmode_Colors::YELLOW,
                drivers::vga::VGA_Textmode_Colors::BLACK
            );

            static const char* error_message = "echo: missing argument\n\n";
            runtime::text_output.put_string(error_message);

            runtime::text_output.set_text_color(
                drivers::vga::VGA_Textmode_Colors::LIGHT_GREY,
                drivers::vga::VGA_Textmode_Colors::BLACK
            );

            return;
        }

        for (const auto& symbol : arguments) [[likely]] {
            if (symbol == null_terminator) [[unlikely]]
                break;
            
            runtime::text_output.put_char(symbol);
        }

        const char new_line = '\n';
        runtime::text_output.put_char(new_line);
        runtime::text_output.put_char(new_line);
    }

    inline void create(const runtime::Array<char, 64>& arguments) noexcept {
        const char null_terminator = '\0';

        if (arguments[0] == null_terminator) [[unlikely]] {
            runtime::text_output.set_text_color(
                drivers::vga::VGA_Textmode_Colors::YELLOW,
                drivers::vga::VGA_Textmode_Colors::BLACK
            );

            runtime::text_output.put_string("create: missing argument\n\n");

            runtime::text_output.set_text_color(
                drivers::vga::VGA_Textmode_Colors::LIGHT_GREY,
                drivers::vga::VGA_Textmode_Colors::BLACK
            );

            return;
        }

        runtime::Array<char, 59> file_name{};
        runtime::Array<char, 4> file_format{};

        uint32_t file_name_index = 0;
        uint32_t file_format_index = 0;
        bool is_file_name = true;

        for (uint32_t i = 0; i < arguments.size(); i++) [[likely]] {
            if (arguments[i] == '.') {
                is_file_name = false;
                continue;
            }

            if (arguments[i] == null_terminator)
                break;

            if (is_file_name && (file_name_index < file_name.size()))
                file_name[file_name_index++] = arguments[i];
            else if (file_format_index < file_format.size())
                file_format[file_format_index++] = arguments[i];
        }

        kernel::filesystem::mofs.create_file(file_name.data(), file_format.data());

        runtime::text_output.put_char('\n');
    }
} // namespace shell::commands
