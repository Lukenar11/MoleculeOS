/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file contains the internal implementation of 
    one of the terminal commands available in MoleculeOS.

    This command clears the screen and displays a helpful explanation 
    and list of all commands and syntax.

    The commands can be called by the shell interpreter from a table generated at compile time, 
    where each entry contains a specific hash 
    and a function call to the command's internal implementation.

NOTES:
*/

#include "commands/help.hpp"

namespace shell::commands
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
} // namespace shell::commands
