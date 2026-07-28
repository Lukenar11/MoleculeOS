/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file contains the internal implementation of 
    one of the terminal commands available in MoleculeOS.

    This command clears the screen and displays a helpful explanation 
    and list of all commands and syntax.

    The commands can be called by the 
    shell interpreter from a table generated at compile time, 
    where each entry contains a specific hash and a function call to the 
    command's internal implementation.

NOTES:
*/

#include "cmds/help.hpp"

namespace shell::commands
{
    void help() noexcept {
        static const char help_message[] = {
            "Command syntax:\n"
            "\t- [Command]\n"
            "\t- [Command] [Argument]\n"
            "\t- [Command] [Argument] [Argument]\n"
            "\nAvailable commands:\n"
            "\t- help (displays a command-line explanation) | help\n"
            "\t- info (displays the MoleculeOS startup screen) | info\n"
            "\t- clear (clears the screen) | clear\n"
            "\t- reboot (restart the Computer) | reboot\n"
            "\t- shutdown (turn the Computer off) | shutdown\n"
            "\t- echo (displays a message or handle file streams)\n"
            "\t\t| (write [Message])                            echo [Message]\n"
            "\t\t| (write [Message] to [File])                  echo [Message] > [File]\n"
            "\t\t| (write the content of [File] to the console) echo < [File]\n"
            "\t- create (creates a file)\n"
            "\t\t| (create a file with default size (1KiB)) create [File]\n"
            "\t\t| (create a file with custom default size) create [File] [ByteSize]\n"
            "\t- list (lists all files) | list\n"
            "\t- remove (deletes a file) | remove [File]\n"
            "\n"
        };

        runtime::Text_Output::reset();
        runtime::Text_Output::put_string(help_message);
    }
} // namespace shell::commands
