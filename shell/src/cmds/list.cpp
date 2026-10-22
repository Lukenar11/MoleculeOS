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

        stdlib::console::reset();
        stdlib::console::put_string("Files:\n");

        File_Entry file_entry;
        for (uint32_t i = 0; i < FILE_TABLE_ENTRYS; i++) [[likely]] {
            file_entry = get_file_entry(i);

            if (!file_entry.file_data_ptr) {
                continue;
            }

            if (file_entry.file_name[0] == '\0') [[unlikely]] {
                static const char* error_message = "\t- [INVALID FILE]\n";
                stdlib::console::put_string(error_message);
                continue;
            }

            stdlib::console::put_string("\t- ");
            stdlib::console::put_string(file_entry.file_name.data());

            if (file_entry.file_format[0] != '\0') {
                stdlib::console::put_char('.');
                stdlib::console::put_string(file_entry.file_format.data());
            }

            stdlib::console::put_string(" (");
            stdlib::console::put_uint(file_entry.file_byte_size);
            stdlib::console::put_string(" Bytes)\n");
        }

        stdlib::console::put_char('\n');
    }
} // namespace shell::commands
