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
    Some shell command implementations are intentionally 
    only implemented in the header, 
    as they are quite small, so that the compiler can inline them.
*/

#include "interpreter/commands.hpp"

namespace shell::interpreter
{
    void help() noexcept {
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
            "\t- list (lists all files) | list\n"
            "\t- remove (deletes a file) | remove [FileName] /\n"
            "\t\t\t\t\t\t\t\tremove [FileName].[FileFormat]\n"
            "\n"
        };

        runtime::text_output.reset();
        runtime::text_output.put_string(help_message);
    }

    void info() noexcept {
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

        runtime::text_output.reset();
        runtime::text_output.put_string(info_message);
    }

    void echo(const runtime::Array<char, 64>& arguments) noexcept {
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

    void create(const runtime::Array<char, 64>& arguments) noexcept {
        static const char* command_name = "create: ";

        Parsed_File_Name parsed = parse_filename(arguments);
        if (parsed.error.data()[0] != '\0') [[unlikely]] {
            print_command_error(command_name);
            print_command_error(parsed.error.data());

            command_end();
            return;
        }

        auto* inode = kernel::filesystem::mofs.create_file(
            parsed.name.data(), 
            parsed.format.data()
        );
        if (!inode) [[unlikely]] {
            static const char* error_message = "file does already exists";
            print_command_error(command_name);
            print_command_error(error_message);

            command_end();
            return;
        }

        command_end();
    }

    void list() noexcept {
        runtime::text_output.reset();
        runtime::text_output.put_string("Files:\n");

        const auto& inodes = kernel::filesystem::mofs.get_inodes();
        for (const auto& inode : inodes) [[likely]] {
            if (!inode.in_use)
                continue;

            if (inode.name[0] == '\0') {
                static const char* error_message = "\t- [INVALID INODE]\n";
                runtime::text_output.put_string(error_message);
                continue;
            }

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

    void remove(const runtime::Array<char, 64>& arguments) noexcept {
        static const char* command_name = "remove: ";

        Parsed_File_Name parsed = parse_filename(arguments);
        if (parsed.error.data()[0] != '\0') [[unlikely]] {
            print_command_error(command_name);
            print_command_error(parsed.error.data());

            command_end();
            return;
        }

        auto* inode = kernel::filesystem::mofs.get_inode_by_name_and_format(
            parsed.name.data(),
            parsed.format.data()
        );

        if (!inode) [[unlikely]] {
            static const char* error_message = "file does not exist";
            print_command_error(command_name);
            print_command_error(error_message);

            command_end();
            return;
        }

        kernel::filesystem::mofs.delete_file(inode);

        command_end();
    }
} // namespace shell::interpreter
