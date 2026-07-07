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
        runtime::Text_Output::reset();
        runtime::Text_Output::put_string("Files:\n");

        const auto& inodes = kernel::filesystem::MoleculeOS_File_System::get_inodes();
        for (const auto& inode : inodes) {
            if (!inode.in_use)
                continue;

            if (inode.name[0] == '\0') [[unlikely]] {
                static const char* error_message = "\t- [INVALID INODE]\n";
                runtime::Text_Output::put_string(error_message);
                continue;
            }

            runtime::Text_Output::put_string("\t- ");
            runtime::Text_Output::put_string(inode.name);

            if (inode.format[0] != '\0') {
                runtime::Text_Output::put_char('.');
                runtime::Text_Output::put_string(inode.format);
            }

            runtime::Text_Output::put_char('\n');
        }

        runtime::Text_Output::put_char('\n');
    }
} // namespace shell::commands
