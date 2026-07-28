/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file contains the internal implementation of 
    one of the terminal commands available in MoleculeOS.

    This command prints text to the screen or handle file streams.

    The commands can be called by the 
    shell interpreter from a table generated at compile time, 
    where each entry contains a specific hash and a function call to the 
    command's internal implementation.

NOTES:
*/

#include "cmds/echo.hpp"

namespace shell::commands
{
    auto& extract_file_instream_befor_redict_operator(const char* redict_operator_pos, 
                                                      const runtime::Array<char, 64>& args)
                                                      noexcept {
        const char null_char = '\0';
        static runtime::Array<char, 64> file_instream;
        file_instream.fill(null_char);

        if (!redict_operator_pos) [[unlikely]]
            return file_instream;

        uint32_t j = 0;

        uint32_t file_instream_end = redict_operator_pos - args.begin();
        for (uint32_t i = 0; i < file_instream_end; i++) {
            if (args[i] == null_char) [[unlikely]]
                break;

            file_instream[j++] = args[i];
        }

        file_instream[j] = null_char;
        return file_instream;
    }

    auto& extract_filename_after_input_redirect(const char* redict_operator_pos,
                                                const runtime::Array<char,64>& args)
                                                noexcept {
        const char null_char  = '\0';
        const char space_char = ' ';

        static runtime::Array<char,64> filename;
        filename.fill(null_char);

        if (!redict_operator_pos) [[unlikely]]
            return filename;

        uint32_t i = (redict_operator_pos - args.begin()) + 1;

        while (args[i] == space_char) 
            i++;

        if (args[i] == null_char) {
            shell::commands::print_command_error("echo: missing filename\n");
            return filename;
        }

        uint32_t j = 0;
        for (; i < args.size(); i++) {
            char argument = args[i];
            if ((argument == space_char) || (argument == null_char))
                break;
        
            filename[j++] = argument;
        }

        filename[j] = null_char;
        return filename;
    }

    auto& extract_text_after_redict_operator(const char* redict_operator_pos, 
                                             const runtime::Array<char, 64>& args)
                                             noexcept {
        const char null_char  = '\0';
        const char space_char = ' ';

        static runtime::Array<char, 64> filename;
        filename.fill(null_char);

        if (!redict_operator_pos) [[unlikely]]
            return filename;

        uint32_t i = (redict_operator_pos - args.begin()) + 1;
        while (args[i] == space_char)
            i++;

        if (args[i] == null_char) {
            shell::commands::print_command_error("echo: missing filename\n");

            filename[0] = null_char;
            return filename;
        }

        uint32_t j = 0;
        for (; i < args.size(); i++) {
            const char argument = args[i];
            if ((argument == space_char) || (argument == null_char)) [[unlikely]]
                break;

            filename[j++] = argument;
        }

        filename[j] = null_char;
        return filename;
    }

    void handle_file_instream(const runtime::Array<char, 64>& filename,
                              const runtime::Array<char, 64>& file_instream) 
                              noexcept {
        using namespace kernel::filesys;

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

        I_Node* inode = MoleculeOS_File_System_2::find_file(parsed_filename.name.data(),
                                                            parsed_filename.format.data());
        if (!inode)
            inode = MoleculeOS_File_System_2::create_file(parsed_filename.name.data(),
                                                          parsed_filename.format.data(),
                                                          file_instream.size());

        MoleculeOS_File_System_2::append_file(inode,
                                              reinterpret_cast<const uint8_t*>(
                                                file_instream.data()
                                              ),
                                              file_instream.size());
    }

    void handle_file_outstream(const runtime::Array<char,64>& filename) noexcept {
        using namespace kernel::filesys;
        using namespace kernel::heap;

        const char* command_name = "echo: ";

        Parsed_File_Name parsed_filename = parse_filename(filename);
        if (parsed_filename.error[0] != '\0') {
            print_command_error(command_name);
            print_command_error(parsed_filename.error.data());
            return;
        }

        I_Node* inode = MoleculeOS_File_System_2::find_file(parsed_filename.name.data(),
                                                            parsed_filename.format.data());
        if (!inode) {
            print_command_error(command_name);
            print_command_error("file not found\n");
            return;
        }

        const uint32_t size = inode->used_data_byte_size + 1;
        uint8_t* buffer = static_cast<uint8_t*>(Block_Allocator::allocate(size));
        if (!buffer) {
            print_command_error(command_name);
            print_command_error("heap allocation failed\n");

            Block_Allocator::deallocate(buffer);
            return;
        }

        if (!MoleculeOS_File_System_2::read_file(inode,
                                                 buffer,
                                                 size,
                                                 0,
                                                 inode->used_data_byte_size)) {
            print_command_error(command_name);
            print_command_error("could not read file\n");

            Block_Allocator::deallocate(buffer);
            return;
        }

        buffer[inode->used_data_byte_size] = '\0';

        for (uint32_t i = 0; buffer[i] != '\0'; i++)
            runtime::Text_Output::put_char(buffer[i]);

        Block_Allocator::deallocate(buffer);
    }

    void echo(const runtime::Array<char, 64>& args) noexcept {
        const char* command_name = "echo: ";

        if (args[0] == '\0') [[unlikely]] {
            print_command_error(command_name);
            print_command_error("missing argument");

            command_end();
            return;
        }

        static runtime::Array<char, 64> file_stream;
        static runtime::Array<char, 64> filename;
        const char null_char = '\0';

        const char* file_outstrem_redict_operator_pos = runtime::String_Manipulation::
                                                        find_char_in_string(args.begin(),
                                                                            '<');

        if (file_outstrem_redict_operator_pos != nullptr) {
            const char* pos = runtime::String_Manipulation::
                              find_char_in_string(file_outstrem_redict_operator_pos + 1, '<');

            if (pos != nullptr) [[unlikely]] {
                print_command_error(command_name);
                print_command_error("multiple redirect operators\n");

                command_end();
                return;
            }
        
            filename = extract_filename_after_input_redirect(file_outstrem_redict_operator_pos,
                                                             args);
        
            handle_file_outstream(filename);
        
            filename.fill(null_char);
            file_stream.fill(null_char);
        
            command_end();
            return;
        }

        const char* file_intstrem_redict_operator_pos = 
            runtime::String_Manipulation::find_char_in_string(
                args.begin(), 
                '>'
            );
        if (file_intstrem_redict_operator_pos != nullptr) {
            const char* pos = runtime::String_Manipulation::
                              find_char_in_string(file_intstrem_redict_operator_pos + 1,
                                                  '>');
            if (pos != nullptr) [[unlikely]] {
                print_command_error(command_name);
                print_command_error("multiple redirect operators\n");

                command_end();
                return;
            }
            
            file_stream = 
                extract_file_instream_befor_redict_operator(file_intstrem_redict_operator_pos,
                                                            args);

            filename = extract_text_after_redict_operator(file_intstrem_redict_operator_pos, 
                                                          args);

            handle_file_instream(filename, file_stream);

            filename.fill(null_char);
            file_stream.fill(null_char);

            command_end();
            return;
        }

        runtime::Text_Output::put_string(args.data());

        command_end();
    }
} // namespace shell::commands
