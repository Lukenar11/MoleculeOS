/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file contains the internal implementation of 
    one of the terminal commands available in MoleculeOS.

    This command prints text to the screan or handle file streams.

    The commands can be called by the 
    shell interpreter from a table generated at compile time, 
    where each entry contains a specific hash and a function call to the 
    command's internal implementation.

NOTES:
*/

#include "commands/echo.hpp"

namespace
{
    auto& extract_file_instream_befor_redict_operator(const char* redict_operator_pos, 
                                                      const runtime::Array<char, 64>& arguments)
                                                      noexcept {
        const char null_char = '\0';
        static runtime::Array<char, 64> file_instream;
        file_instream.fill(null_char);

        if (!redict_operator_pos) [[unlikely]]
            return file_instream;

        uint32_t j = 0;

        uint32_t file_instream_end = redict_operator_pos - arguments.begin();
        for (uint32_t i = 0; i < file_instream_end; i++) {
            if (arguments[i] == null_char) [[unlikely]]
                break;

            file_instream[j++] = arguments[i];
        }

        file_instream[j] = null_char;
        return file_instream;
    }

    auto& extract_filename_after_input_redirect(const char* redict_operator_pos,
                                                const runtime::Array<char,64>& arguments)
                                                noexcept {
        const char null_char = '\0';
        const char space_char = ' ';
        const uint32_t null = 0;

        static runtime::Array<char,64> filename;
        filename.fill(null_char);

        if (!redict_operator_pos) [[unlikely]]
            return filename;

        uint32_t i = (redict_operator_pos - arguments.begin()) + 1;

        while (arguments[i] == space_char) 
            i++;

        if (arguments[i] == null_char) {
            shell::commands::print_command_error("echo: missing filename\n");
            return filename;
        }

        uint32_t j = null;
        for (; i < arguments.size(); i++) {
            char argument = arguments[i];
            if ((argument == space_char) || (argument == null_char))
                break;
        
            filename[j++] = argument;
        }

        filename[j] = null_char;
        return filename;
    }

    auto& extract_text_after_redict_operator(const char* redict_operator_pos, 
                                             const runtime::Array<char, 64>& arguments)
                                             noexcept {
        const char null_char = '\0';
        const char spache_char = ' ';

        static runtime::Array<char, 64> filename;
        filename.fill(null_char);

        if (!redict_operator_pos) [[unlikely]]
            return filename;

        uint32_t i = (redict_operator_pos - arguments.begin()) + 1;
        while (arguments[i] == spache_char)
            i++;

        if (arguments[i] == null_char) {
            shell::commands::print_command_error("echo: missing filename\n");

            filename[0] = null_char;
            return filename;
        }

        uint32_t j = 0;
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
        static shell::commands::Parsed_File_Name parsed_filename;
        parsed_filename.name.fill('\0');
        parsed_filename.format.fill('\0');
        parsed_filename.error.fill('\0');

        parsed_filename = shell::commands::parse_filename(filename);
        if (parsed_filename.error.data()[0] != '\0') [[unlikely]] {
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
            runtime::string_manip.get_string_length(file_instream.data())
        );
    }
}

void handle_file_outstream(const runtime::Array<char,64>& filename) noexcept {
    const char* command_name = "echo: ";

    static runtime::Array<char, kernel::filesystem::MAX_FILE_SIZE> buffer;
    buffer.fill('\0');

    static shell::commands::Parsed_File_Name parsed_filename;
    parsed_filename.name.fill('\0');
    parsed_filename.format.fill('\0');
    parsed_filename.error.fill('\0');

    parsed_filename = shell::commands::parse_filename(filename);
    if (parsed_filename.error[0] != '\0') {
        shell::commands::print_command_error(command_name);
        shell::commands::print_command_error(parsed_filename.error.data());

        return;
    }

    auto* inode = kernel::filesystem::mofs.get_inode_by_name_and_format(
        parsed_filename.name.data(),
        parsed_filename.format.data()
    );
    if (!inode) {
        shell::commands::print_command_error(command_name);
        shell::commands::print_command_error("file not found\n");

        return;
    }

    const uint32_t needed_size = inode->size + 1;
    if (needed_size > buffer.size()) {
        shell::commands::print_command_error(command_name);
        shell::commands::print_command_error("file too large\n");

        return;
    }

    if (!kernel::filesystem::mofs.get_file_content_as_string(
            inode,
            buffer.data(),
            needed_size
        )) {
        shell::commands::print_command_error(command_name);
        shell::commands::print_command_error("could not read file\n");

        return;
    }

    for (uint32_t i = 0; buffer[i] != '\0'; i++)
        runtime::text_output.put_char(buffer[i]);
}

namespace shell::commands
{
    void echo(const runtime::Array<char, 64>& arguments) noexcept {
        const char* command_name = "echo: ";

        if (arguments[0] == '\0') [[unlikely]] {
            print_command_error(command_name);
            print_command_error("missing argument");

            command_end();
            return;
        }

        static runtime::Array<char, 64> file_stream;
        static runtime::Array<char, 64> filename;
        const char null_char = '\0';

        const char* file_outstrem_redict_operator_pos = 
            runtime::string_manip.find_char_in_string(
                arguments.begin(), 
                '<'
            );
        if (file_outstrem_redict_operator_pos != nullptr) {
            const char* pos = runtime::string_manip.find_char_in_string(
                file_outstrem_redict_operator_pos + 1, 
                '<'
            );
            if (pos != nullptr) [[unlikely]] {
                print_command_error(command_name);
                print_command_error("multiple redirect operators\n");

                command_end();
                return;
            }
        
            filename = extract_filename_after_input_redirect(
                file_outstrem_redict_operator_pos,
                arguments
            );
        
            handle_file_outstream(filename);
        
            filename.fill(null_char);
            file_stream.fill(null_char);
        
            command_end();
            return;
        }

        const char* file_intstrem_redict_operator_pos = 
            runtime::string_manip.find_char_in_string(
                arguments.begin(), 
                '>'
            );
        if (file_intstrem_redict_operator_pos != nullptr) {
            const char* pos = runtime::string_manip.find_char_in_string(
                file_intstrem_redict_operator_pos + 1,
                '>'
            );
            if (pos != nullptr) [[unlikely]] {
                print_command_error(command_name);
                print_command_error("multiple redirect operators\n");

                command_end();
                return;
            }
            
            file_stream = extract_file_instream_befor_redict_operator(
                file_intstrem_redict_operator_pos, 
                arguments
            );

            filename = extract_text_after_redict_operator(
                file_intstrem_redict_operator_pos, 
                arguments
            );

            handle_file_instream(filename, file_stream);

            filename.fill(null_char);
            file_stream.fill(null_char);

            command_end();
            return;
        }

        runtime::text_output.put_string(arguments.data());

        command_end();
    }
} // namespace shell::commands
