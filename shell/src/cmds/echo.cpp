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
    stdlib::Array<char,64>& extract_echo_text(const stdlib::Array<char,64>& args,
                                              const char* redirect_pos) 
                                              noexcept {
        static stdlib::Array<char,64> text;
        text.fill('\0');

        uint32_t start = 0;
        if (args[0] == 'e' && args[1] == 'c' && args[2] == 'h' && args[3] == 'o') {
            start = 4;
            while (args[start] == ' ')
                start++;
        }

        uint32_t end = redirect_pos - args.begin();
        uint32_t j   = 0;
        for (uint32_t i = start; i < end; i++) {
            if (args[i] == '\0')
                break;

            text[j++] = args[i];
        }

        text[j] = '\0';
        return text;
    }

    auto& extract_filename_after_input_redirect(const char* redict_operator_pos,
                                                const stdlib::Array<char,64>& args)
                                                noexcept {
        const char null_char  = '\0';
        const char space_char = ' ';

        static stdlib::Array<char,64> filename;
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
                                             const stdlib::Array<char, 64>& args)
                                             noexcept {
        const char null_char  = '\0';
        const char space_char = ' ';

        static stdlib::Array<char, 64> filename;
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

    void handle_file_instream(const stdlib::Array<char, 64>& filename,
                              const stdlib::Array<char, 64>& file_instream) 
                              noexcept {
        using namespace kernel::filesys;
        using namespace stdlib;

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

        kernel::filesys::Handle handle;
        kernel::filesys::open_file(handle, 
                                   parsed_filename.name.data(),
                                   parsed_filename.format.data(),
                                   kernel::filesys::Handle_Flags::WRITE |
                                   kernel::filesys::Handle_Flags::CREATE |
                                   kernel::filesys::Handle_Flags::APPEND);

        uint32_t length;
        stdlib::string::get_length(length, file_instream.data());
        const auto& instream = reinterpret_cast<const uint8_t*>(file_instream.data());

        kernel::filesys::write_file(handle, 
                                    instream, 
                                    sizeof(file_instream.data()) / sizeof(char), 
                                    0, 
                                    length - 1);

        kernel::filesys::close_file(handle);
    }

    void handle_file_outstream(const stdlib::Array<char,64>& filename) noexcept {
        using namespace kernel::filesys;
        using namespace kernel::heap;

        const char* command_name = "echo: ";

        Parsed_File_Name parsed_filename = parse_filename(filename);
        if (parsed_filename.error[0] != '\0') {
            print_command_error(command_name);
            print_command_error(parsed_filename.error.data());
            return;
        }

        kernel::filesys::Handle handle;
        status_t status = kernel::filesys::open_file(handle, 
                                                     parsed_filename.name.data(),
                                                     parsed_filename.format.data(),
                                                     kernel::filesys::Handle_Flags::READ);
        if (status != status::SUCCESS) {
            print_command_error(command_name);
            print_command_error("file not found\n");
            return;
        }
    
        uint32_t size;
        void* ptr;

        kernel::filesys::get_file_size(size, handle);
        size += 1;

        allocate(ptr, size);
        uint8_t* buffer = static_cast<uint8_t*>(ptr);
        if (!buffer) {
            print_command_error(command_name);
            print_command_error("heap allocation failed\n");
            return;
        }


        status = kernel::filesys::read_file(handle, buffer, size, 0, size);
        if (status != status::SUCCESS) [[unlikely]] {
            print_command_error(command_name);
            print_command_error("could not read file\n");

            deallocate(ptr);
            return;
        }

        buffer[size] = '\0';

        for (uint32_t i = 0; buffer[i] != '\0'; i++)
            stdlib::console::put_char(buffer[i]);

        deallocate(ptr);
        kernel::filesys::close_file(handle);
    }

    void echo(const stdlib::Array<char, 64>& args) noexcept {
        const char* command_name = "echo: ";

        if (args[0] == '\0') [[unlikely]] {
            print_command_error(command_name);
            print_command_error("missing argument");

            command_end();
            return;
        }

        static stdlib::Array<char, 64> file_stream;
        static stdlib::Array<char, 64> filename;
        const char null_char = '\0';

        const char* file_outstrem_redict_operator_pos;
        stdlib::string::find_char(file_outstrem_redict_operator_pos, args.begin(), '<');

        if (file_outstrem_redict_operator_pos != nullptr) {
            const char* pos;
            stdlib::string::find_char(pos, file_outstrem_redict_operator_pos + 1, '<');

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

        const char* file_intstrem_redict_operator_pos;
        stdlib::string::find_char(file_intstrem_redict_operator_pos, args.begin(), '>');
        if (file_intstrem_redict_operator_pos != nullptr) {
            const char* pos;
            stdlib::string::find_char(pos, file_intstrem_redict_operator_pos + 1, '>');
            if (pos != nullptr) [[unlikely]] {
                print_command_error(command_name);
                print_command_error("multiple redirect operators\n");

                command_end();
                return;
            }
            
            file_stream = extract_echo_text(args, file_intstrem_redict_operator_pos);
            filename    = extract_text_after_redict_operator(file_intstrem_redict_operator_pos, 
                                                             args);

            handle_file_instream(filename, file_stream);

            filename.fill(null_char);
            file_stream.fill(null_char);

            command_end();
            return;
        }

        stdlib::console::put_string(args.data());
        command_end();
    }
} // namespace shell::commands
