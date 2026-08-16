/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file contains the internal implementation of 
    one of the terminal commands available in MoleculeOS.

    This command checks if a file already exists.
    
    The commands can be called by the 
    shell interpreter from a table generated at compile time, 
    where each entry contains a specific hash and a function call to the 
    command's internal implementation.

NOTES:
*/

#include "cmds/exists.hpp"

namespace shell::commands
{
    void exists(const stdlib::Array<char, 64>& args) noexcept {
        static const char* command_name = "exists: ";

        Parsed_File_Name& parsed_filename = parse_filename(args);
        if (parsed_filename.error.data()[0] != '\0') {
            print_command_error(command_name);
            print_command_error(parsed_filename.error.data());

            command_end();
            return;
        }

        kernel::filesys::File_Header* file_header;
        kernel::filesys::MoleculeOS_File_System_2::find_file(file_header,
                                                             parsed_filename.name.data(), 
                                                             parsed_filename.format.data());
        if (!file_header) {
            print_command_error(command_name);
            print_command_error("File does not exist!");

        }
        else {
            print_command_error(command_name);
            print_command_error("File exists!");
        }

        command_end();
    }
} // namespace shell::commands
