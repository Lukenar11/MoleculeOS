/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file contains the internal implementation of 
    one of the terminal commands available in MoleculeOS.

    This command copy the content of a file to another file.
    
    The commands can be called by the 
    shell interpreter from a table generated at compile time, 
    where each entry contains a specific hash and a function call to the 
    command's internal implementation.

NOTES:
*/

#include "cmds/copy.hpp"

namespace shell::commands
{
    void copy(const stdlib::Array<char, 64>& args) noexcept {
        using namespace kernel::filesys;

        static const char* command_name = "copy: ";

        if (args[0] == '\0') {
            print_command_error(command_name);
            print_command_error("missing arguments");
            command_end();
            return;
        }

        uint32_t split_index = 0;
        while (split_index < args.size() && args[split_index] != ' ')
            split_index++;

        if (split_index == 0 || split_index >= args.size()) {
            print_command_error(command_name);
            print_command_error("invalid arguments");
            command_end();
            return;
        }

        static stdlib::Array<char, 64> src_filename;
        src_filename.fill('\0');

        for (uint32_t i = 0; i < split_index; i++)
            src_filename[i] = args[i];

        static stdlib::Array<char, 64> dest_filename;
        dest_filename.fill('\0');

        uint32_t dest_index = 0;
        for (uint32_t i = split_index + 1; i < args.size(); i++) {
            if (args[i] == '\0')
                break;

            dest_filename[dest_index++] = args[i];
        }

        stdlib::console::put_char('\n');

        Parsed_File_Name src_parsed = parse_filename(src_filename);
        if (src_parsed.error.data()[0] != '\0') {
            print_command_error(command_name);
            print_command_error(src_parsed.error.data());
            command_end();
            return;
        }

        Parsed_File_Name dest_parsed = parse_filename(dest_filename);
        if (dest_parsed.error.data()[0] != '\0') {
            print_command_error(command_name);
            print_command_error(dest_parsed.error.data());
            command_end();
            return;
        }

        kernel::filesys::Handle src_handle;
        kernel::filesys::open_file(src_handle, 
                                   src_parsed.name.data(), 
                                   src_parsed.format.data(), 
                                   kernel::filesys::Handle_Flags::READ);

        kernel::filesys::Handle dest_handle;
        kernel::filesys::open_file(dest_handle, 
                                   dest_parsed.name.data(), 
                                   dest_parsed.format.data(), 
                                   kernel::filesys::Handle_Flags::WRITE | 
                                   kernel::filesys::Handle_Flags::CREATE |
                                   kernel::filesys::Handle_Flags::TRUNCATE);

        uint32_t size;
        void* buffer;

        kernel::filesys::get_file_size(size, src_handle);
        kernel::heap::allocate(buffer, size);

        kernel::filesys::read_file(src_handle, 
                                   reinterpret_cast<uint8_t*>(buffer), 
                                   size, 
                                   0, 
                                   size - 1);

        status_t status = kernel::filesys::write_file(dest_handle, 
                                                      reinterpret_cast<const uint8_t*>(buffer), 
                                                      size, 
                                                      0, 
                                                      size - 1);
        if (status != status::SUCCESS) {
            print_command_error(command_name);
            print_command_error("copy failed");
            command_end();
            return;
        }

        kernel::filesys::close_file(src_handle);
        kernel::filesys::close_file(dest_handle);
        command_end();
    }
}
