/**
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

#include "cmds/list.hpp"

namespace shell::commands
{
    void list() noexcept {
        runtime::Text_Output::reset();
        runtime::Text_Output::put_string("Files:\n");

        const auto& inodes = kernel::filesys::MoleculeOS_File_System_2::get_inode_table();
        for (const auto& inode : inodes) {
            if (inode.file_data_ptr == nullptr)
                continue;

            if (inode.file_name[0] == '\0') [[unlikely]] {
                static const char* error_message = "\t- [INVALID FILE]\n";
                runtime::Text_Output::put_string(error_message);
                continue;
            }

            runtime::Text_Output::put_string("\t- ");
            runtime::Text_Output::put_string(inode.file_name.data());

            if (inode.file_format[0] != '\0') {
                runtime::Text_Output::put_char('.');
                runtime::Text_Output::put_string(inode.file_format.data());
            }

            runtime::Text_Output::put_string(" (");
            runtime::Text_Output::put_uint(inode.file_byte_size);
            runtime::Text_Output::put_string(" Bytes)\n");
        }

        runtime::Text_Output::put_char('\n');
    }
} // namespace shell::commands
