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
        using namespace kernel::filesys;

        stdlib::Text_Output::reset();
        stdlib::Text_Output::put_string("Files:\n");

        File_Header file_header;
        for (uint32_t i = 0; i < FILE_HEADER_TABLE_ENTRYS; i++) [[likely]] {
            file_header = MoleculeOS_File_System_2::get_file_header_entry(i);

            if (!file_header.file_data_ptr) {
                continue;
            }

            if (file_header.file_name[0] == '\0') [[unlikely]] {
                static const char* error_message = "\t- [INVALID FILE]\n";
                stdlib::Text_Output::put_string(error_message);
                continue;
            }

            stdlib::Text_Output::put_string("\t- ");
            stdlib::Text_Output::put_string(file_header.file_name.data());

            if (file_header.file_format[0] != '\0') {
                stdlib::Text_Output::put_char('.');
                stdlib::Text_Output::put_string(file_header.file_format.data());
            }

            stdlib::Text_Output::put_string(" (");
            stdlib::Text_Output::put_uint(file_header.file_byte_size);
            stdlib::Text_Output::put_string(" Bytes)\n");
        }

        stdlib::Text_Output::put_char('\n');
    }
} // namespace shell::commands
