/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file contains the internal implementation of 
    one of the terminal commands available in MoleculeOS.

    This command deletes a specific file.

    The commands can be called by the 
    shell interpreter from a table generated at compile time, 
    where each entry contains a specific hash and a function call to the 
    command's internal implementation.

NOTES:
*/

#include "commands/remove.hpp"

namespace shell::commands
{
    void remove(const runtime::Array<char, 64>& arguments) noexcept {
        static const char* command_name = "remove: ";

        Parsed_File_Name parsed = parse_filename(arguments);
        if (parsed.error.data()[0] != '\0') [[unlikely]] {
            print_command_error(command_name);
            print_command_error(parsed.error.data());

            command_end();
            return;
        }

        auto* inode = kernel::filesystem::MoleculeOS_File_System::
                      get_inode_by_name_and_format(parsed.name.data(),
                                                  parsed.format.data());

        if (!inode) [[unlikely]] {
            static const char* error_message = "file does not exist";
            print_command_error(command_name);
            print_command_error(error_message);

            command_end();
            return;
        }

        kernel::filesystem::MoleculeOS_File_System::delete_file(inode);
        command_end();
    }
} // namespace shell::commands
