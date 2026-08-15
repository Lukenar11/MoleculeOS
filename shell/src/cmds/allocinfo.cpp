/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file contains the internal implementation of 
    one of the terminal commands available in MoleculeOS.

    This command prints the file allocation info.

    The commands can be called by the 
    shell interpreter from a table generated at compile time, 
    where each entry contains a specific hash and a function call to the 
    command's internal implementation.

NOTES:
    Since this function is so small, it is intentionally placed in the 
    header so that the compiler can execute it inline.
*/

#include "cmds/allocinfo.hpp"

namespace shell::commands
{
    void allocinfo(const stdlib::Array<char, 64>& args) noexcept {
        using namespace kernel::filesys;

        static const char* command_name = "allocinfo: ";
        
        Parsed_File_Name parsed_filename;
        parsed_filename.error.fill('\0');
        parsed_filename.name.fill('\0');
        parsed_filename.format.fill('\0');

        parsed_filename = parse_filename(args);
        if (parsed_filename.error[0] != '\0') {
            print_command_error(command_name);
            print_command_error(parsed_filename.error.data());
            command_end();
            return;
        }

        File_Header* file_header = MoleculeOS_File_System_2::find_file(parsed_filename.name.data(),
                                                                 parsed_filename.format.data());
        if (!file_header) {
            print_command_error(command_name);
            print_command_error("file not found");

            command_end();
            return;
        }

        uint32_t index = 0;
        uint32_t blocks = 0;
        if (!kernel::heap::Block_Allocator::get_allocation_info(index,
                                                                blocks,
                                                                file_header->file_data_ptr)) {
            print_command_error(command_name);
            print_command_error("allocation info not available");

            command_end();
            return;
        }

        stdlib::Text_Output::put_string("Index: ");
        stdlib::Text_Output::put_uint(index);
        stdlib::Text_Output::put_char('\n');

        stdlib::Text_Output::put_string("Blocks: ");
        stdlib::Text_Output::put_uint(blocks);
        stdlib::Text_Output::put_char('\n');

        command_end();
    }
}
