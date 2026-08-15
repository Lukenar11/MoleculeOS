/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file contains the internal implementation of 
    one of the terminal commands available in MoleculeOS.

    This command creates a file.

    The commands can be called by the 
    shell interpreter from a table generated at compile time, 
    where each entry contains a specific hash and a function call to the 
    command's internal implementation.

NOTES:
*/

#include "cmds/create.hpp"

static void split_file_name_and_size(const stdlib::Array<char, 64>& args,
                                     stdlib::Array<char, 64>& filename,
                                     uint32_t& file_size) 
                                     noexcept {
    using namespace stdlib;

    file_size = 1024;

    String_Manipulation::copy_string(filename.data(), args.data());

    char* filename_ptr = filename.data();

    while (*filename_ptr == ' ') 
        ++filename_ptr;

    if (filename_ptr != filename.data())
        String_Manipulation::copy_string(filename.data(), filename_ptr);

    char* last_space = nullptr;

    for (char* filename_ptr = filename.data(); *filename_ptr != '\0'; ++filename_ptr)
        if (*filename_ptr == ' ')
            last_space = filename_ptr;

    if (last_space == nullptr)
        return;

    char* size_ptr = last_space + 1;
    if (*size_ptr == '\0')
        return;

    for (char* i_ptr = size_ptr; *i_ptr != '\0'; ++i_ptr)
        if (!String_Manipulation::is_digit(*i_ptr))
            return;

    int32_t temp;
    String_Manipulation::string_to_int(temp, size_ptr);
    file_size = static_cast<uint32_t>(temp);

    *last_space = '\0';

    return;
}

namespace shell::commands
{
    void create(const stdlib::Array<char, 64>& args) noexcept {
        using namespace kernel::filesys;
        using namespace stdlib;

        static const char* command_name = "create: ";

        Array<char, 64> filename_args;
        uint32_t file_size;

        split_file_name_and_size(args, filename_args, file_size);

        Parsed_File_Name& parsed_filename = parse_filename(filename_args);
        if (parsed_filename.error.data()[0] != '\0') [[unlikely]] {
            print_command_error(command_name);
            print_command_error(parsed_filename.error.data());

            command_end();
            return;
        }

        File_Header* file_header = MoleculeOS_File_System_2::create_file(parsed_filename.name.data(),
                                                                   parsed_filename.format.data(),
                                                                   file_size);
        if (!file_header) [[unlikely]] {
            static const char* error_message = "file could not be created";

            print_command_error(command_name);
            print_command_error(error_message);

            command_end();
            return;
        }

        command_end();
    }
} // namespace shell::commands
