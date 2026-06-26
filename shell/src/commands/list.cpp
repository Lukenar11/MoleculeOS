/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file contains the internal implementation of 
    one of the terminal commands available in MoleculeOS.

    This command lists all the files.

    The commands can be called by the 
    shell interpreter from a table generated at compile time, 
    where each entry contains a specific hash and a function call to the 
    command's internal implementation.

NOTES:
*/

#include "commands/list.hpp"

namespace shell::commands
{
    void list() noexcept {
        runtime::text_output.reset();
        runtime::text_output.put_string("Files:\n");

        const auto& inodes = kernel::filesystem::mofs.get_inodes();
        for (const auto& inode : inodes) {
            if (!inode.in_use)
                continue;

            if (inode.name[0] == '\0') [[unlikely]] {
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
} // namespace shell::commands
