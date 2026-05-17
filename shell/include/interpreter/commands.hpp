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
            "\n"
            "Available commands:\n"
            "\t- help (displays a command-line explanation) | help\n"
            "\t- info (displays the MoleculeOS startup screen) | info\n"
            "\t- clear (clears the screen) | clear\n"
            "\t- reboot (restart the Computer) | reboot\n"
            "\t- shutdown (turn the Computer off) | shutdown\n"
            "\t- echo (displays a message) | echo [Message]\n"
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
            "\nVersion 0.0.0 (Kernel Mode)\n"
            "\nCopyright (c) 2026 Lukenar11 (Luke Matthes)\n"
            "MIT Licensed\n"
            "https://github.com/Lukenar11/MoleculeOS\n"
            "\n\n"
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
        if (arguments[NULL] == '\0') {
            static const char* error_message = "\necho: missing argument\n";
            runtime::text_output.put_string(error_message);
            return;
        }

        runtime::text_output.put_char('\n');
        for (uint32_t i = NULL; i < arguments.size(); ++i) {
            const char symbol = arguments[i];
            if (symbol == '\0')
                break;
            runtime::text_output.put_char(symbol);
        }

        runtime::text_output.put_char('\n');
    }
} // namespace shell::commands
