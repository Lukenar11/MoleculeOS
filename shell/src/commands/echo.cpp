/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file contains the internal implementation of 
    one of the terminal commands available in MoleculeOS.

    This command prints text to the screan or handle file streams.

    The commands can be called by the shell interpreter from a table generated at compile time, 
    where each entry contains a specific hash 
    and a function call to the command's internal implementation.

NOTES:
*/

#include "commands/echo.hpp"

namespace
{
    auto& extract_file_instream_befor_redict_operator(const char* redict_operator_pos, 
                                                      const runtime::Array<char, 64>& arguments)
                                                      noexcept {
        const char null_char = '\0';
        const uint32_t null = 0;
        static runtime::Array<char, 64> file_instream;
        file_instream.fill(null_char);

        if (!redict_operator_pos) [[unlikely]]
            return file_instream;

        uint32_t j = null;

        uint32_t file_instream_end = redict_operator_pos - arguments.begin();
        for (uint32_t i = null; i < file_instream_end; i++) [[likely]] {
            if (arguments[i] == null_char) [[unlikely]]
                break;

            file_instream[j++] = arguments[i];
        }

        file_instream[j] = null_char;
        return file_instream;
    }

    auto& extract_text_after_redict_operator(const char* redict_operator_pos, 
                                             const runtime::Array<char, 64>& arguments)
                                             noexcept {
        const char null_char = '\0';
        const char spache_char = ' ';
        const uint32_t null = 0;

        static runtime::Array<char, 64> filename;
        filename.fill(null_char);

        if (!redict_operator_pos) [[unlikely]]
            return filename;

        uint32_t i = (redict_operator_pos - arguments.begin()) + 1;
        while (arguments[i] == spache_char) [[likely]]
            i++;

        if (arguments[i] == null_char) [[unlikely]] {
            shell::commands::print_command_error("echo: missing filename\n");

            filename[null] = null_char;
            return filename;
        }

        uint32_t j = null;
        for (; i < arguments.size(); i++) [[unlikely]] {
            const char argument = arguments[i];
            if ((argument == spache_char) || (argument == null_char)) [[unlikely]]
                break;

            filename[j++] = argument;
        }

        filename[j] = null_char;
        return filename;
    }

    void handle_file_instream(const runtime::Array<char, 64>& filename,
                              const runtime::Array<char, 64>& file_instream) 
                              noexcept {
        const char null_char = '\0';
        static shell::commands::Parsed_File_Name parsed_filename;
        parsed_filename.name.fill(null_char);
        parsed_filename.format.fill(null_char);
        parsed_filename.error.fill(null_char);

        parsed_filename = shell::commands::parse_filename(filename);
        if (parsed_filename.error.data()[0] != null_char) [[unlikely]] {
            shell::commands::print_command_error("echo: ");
            shell::commands::print_command_error(parsed_filename.error.data());
            return;
        }
    
        auto* inode = kernel::filesystem::mofs.get_inode_by_name_and_format(
            parsed_filename.name.data(),
            parsed_filename.format.data()
        );
        if (!inode)
            inode = kernel::filesystem::mofs.create_file(
                parsed_filename.name.data(),
                parsed_filename.format.data()
            );

        kernel::filesystem::mofs.set_file_content_as_string(
            inode,
            file_instream.data(),
            strlen(file_instream.data())
        );
    }
}

namespace shell::commands
{
    void echo(const runtime::Array<char, 64>& arguments) noexcept {
        if (arguments[0] == '\0') [[unlikely]] {
            print_command_error("echo: missing argument");

            command_end();
            return;
        }

        static runtime::Array<char, 64> file_instream;
        static runtime::Array<char, 64> filename;
        const char null_char = '\0';

        const char* first_redict_operator_pos = strchr(arguments.begin(), '>');
        if (first_redict_operator_pos != nullptr) {
            const char* second = strchr(first_redict_operator_pos + 1, '>');
            if (second != nullptr) [[unlikely]] {
                print_command_error("echo: multiple redirect operators\n");

                command_end();
                return;
            }
            
            file_instream = extract_file_instream_befor_redict_operator(
                first_redict_operator_pos, 
                arguments
            );

            filename = extract_text_after_redict_operator(
                first_redict_operator_pos, 
                arguments
            );

            handle_file_instream(filename, file_instream);

            filename.fill(null_char);
            file_instream.fill(null_char);

            command_end();
            return;
        }

        for (const char& argument : arguments) [[likely]] {
            if (argument == '\0') [[unlikely]]
                break;

            runtime::text_output.put_char(argument);
        }

        command_end();
    }
} // namespace shell::commands
