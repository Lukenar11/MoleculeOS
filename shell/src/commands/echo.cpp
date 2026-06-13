/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file contains the internal implementation of 
    one of the terminal commands available in MoleculeOS.

    The commands can be called by the shell interpreter from a table generated at compile time, 
    where each entry contains a specific hash 
    and a function call to the command's internal implementation.

NOTES:
*/

#include "commands/echo.hpp"

namespace shell::commands
{
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

        command_end();
    }
} // namespace shell::commands
