#include "cmds/copy.hpp"

namespace shell::commands
{
    void copy(const runtime::Array<char, 64>& args) noexcept {
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

        static runtime::Array<char, 64> src_filename;
        src_filename.fill('\0');

        for (uint32_t i = 0; i < split_index; i++)
            src_filename[i] = args[i];

        static runtime::Array<char, 64> dest_filename;
        dest_filename.fill('\0');

        uint32_t dest_index = 0;
        for (uint32_t i = split_index + 1; i < args.size(); i++) {
            if (args[i] == '\0')
                break;

            dest_filename[dest_index++] = args[i];
        }

        runtime::Text_Output::put_char('\n');

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

        if (!MoleculeOS_File_System_2::copy_file(src_parsed.name.data(),
                                                 src_parsed.format.data(),
                                                 dest_parsed.name.data(),
                                                 dest_parsed.format.data())) {
            print_command_error(command_name);
            print_command_error("copy failed");
            command_end();
            return;
        }

        command_end();
    }
}
