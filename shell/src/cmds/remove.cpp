/**
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

#include "cmds/remove.hpp"

namespace shell::commands
{
    void remove(const stdlib::Array<char, 64>& args) noexcept {
        using namespace kernel::filesys;
        
        static const char* command_name = "remove: ";

        Parsed_File_Name& parsed_filename = parse_filename(args);
        if (parsed_filename.error.data()[0] != '\0') [[unlikely]] {
            print_command_error(command_name);
            print_command_error(parsed_filename.error.data());

            command_end();
            return;
        }

        if (MoleculeOS_File_System_2::delete_file(parsed_filename.name.data(),
                                                  parsed_filename.format.data()) 
            != status::SUCCESS) [[unlikely]] {
            static const char* error_message = "file does not exist";
            print_command_error(command_name);
            print_command_error(error_message);

            command_end();
            return;
        }

        command_end();
    }
} // namespace shell::commands
