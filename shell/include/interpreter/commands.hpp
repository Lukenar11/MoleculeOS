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
#include "utils/helpers.hpp"
#include <stdint.h>
#include <text_output.hpp>
#include <array.hpp>
#include <string.h>

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
            "\t- create (creates a file) | create [FileName] /\n"
            "\t\t\t\t\t\t\t\tcreate [FileName].[FileFormat]\n"
            "\t- list (list all files) | list\n"
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
            static const char* error_message = "echo: missing argument\n\n";
            print_command_error(error_message);
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
        const uint32_t null = 0;

        if (arguments[null] == null_terminator) [[unlikely]] {
            static const char* error_message = "create: missing argument\n\n";
            print_command_error(error_message);
            return;
        }

        static runtime::Array<char, 59> file_name;
        static runtime::Array<char, 4> file_format;

        uint32_t file_name_index = null;
        uint32_t file_format_index = null;
        bool is_file_name = true;
    
        for (uint32_t i = null; i < arguments.size(); i++) [[likely]] {
            if (arguments[i] == '.') {
                is_file_name = false;
                continue;
            }

            if (arguments[i] == null_terminator) [[unlikely]]
                break;

            if (is_file_name && !append_char(file_name, file_name_index, arguments[i])) {
                static const char* error_message = "create: filename too long\n\n";
                print_command_error(error_message);
                return;
            }
            
            if (!(is_file_name || append_char(file_format, file_format_index, arguments[i]))) {
                static const char* error_message = "create: format too long\n\n";
                print_command_error(error_message);
                return;
            }
        }

        file_name[file_name_index] = null_terminator;
        file_format[file_format_index] = null_terminator;

        const auto& inodes = kernel::filesystem::mofs.get_inodes();
        for (uint32_t i = null; i < inodes.size(); i++) [[likely]] {
            if (!inodes[i].in_use) [[likely]]
                continue;

            if ((strcmp(file_name.data(), inodes[i].name) == null) && 
                (strcmp(file_format.data(), inodes[i].format) == null)) [[unlikely]] {
                static const char* error_message = "create: file does already exists\n\n";
                print_command_error(error_message);
                return;
            }
        }

        kernel::filesystem::mofs.create_file(file_name.data(), file_format.data());

        file_name.fill(null_terminator);
        file_format.fill(null_terminator);

        runtime::text_output.put_char('\n');
    }

    inline void list() noexcept {
        runtime::text_output.put_string("Files:\n");

        const auto& inodes = kernel::filesystem::mofs.get_inodes();
        for (const auto& inode : inodes) {
            if (!inode.in_use) [[likely]]
                continue;

            runtime::text_output.put_string("\t- ");
            runtime::text_output.put_string(inode.name);

            if (inode.format[0] != '\0') {
                runtime::text_output.put_char('.');
                runtime::text_output.put_string(inode.format);
            }

            runtime::text_output.put_char('\n');
        }

        runtime::text_output.put_char('\n');
    }
} // namespace shell::commands
