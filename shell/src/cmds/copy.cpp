#include "cmds/copy.hpp"

namespace shell::commands
{
    void copy(const runtime::Array<char, 64>& args) noexcept {
        using namespace kernel::filesys;

        static Parsed_File_Name old_parsed_filename;
        old_parsed_filename.error.fill('\0');
        old_parsed_filename.name.fill('\0');
        old_parsed_filename.format.fill('\0');

        static Parsed_File_Name new_parsed_filename;
        new_parsed_filename.error.fill('\0');
        new_parsed_filename.name.fill('\0');
        new_parsed_filename.format.fill('\0');

        static const char* command_name = "rename: ";

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

        static runtime::Array<char, 64> old_filename;
        old_filename.fill('\0');

        for (uint32_t i = 0; i < split_index; i++)
            old_filename[i] = args[i];

        static runtime::Array<char, 64> new_filename;
        new_filename.fill('\0');

        uint32_t new_filename_index = 0;
        for (uint32_t i = split_index + 1; i < args.size(); i++) {
            if (args[i] == '\0')
                break;

            new_filename[new_filename_index++] = args[i];
        }

        runtime::Text_Output::put_char('\n');

        old_parsed_filename = parse_filename(old_filename);
        if (old_parsed_filename.error.data()[0] != '\0') {
            print_command_error(command_name);
            print_command_error(old_parsed_filename.error.data());

            command_end();
            return;
        }

        new_parsed_filename = parse_filename(new_filename);
        if (new_parsed_filename.error.data()[0] != '\0') {
            print_command_error(command_name);
            print_command_error(new_parsed_filename.error.data());

            command_end();
            return;
        }

        if (!MoleculeOS_File_System_2::copy_file(old_parsed_filename.name.data(),
                                                 old_parsed_filename.format.data(),
                                                 new_parsed_filename.name.data(),
                                                 new_parsed_filename.format.data())) {
            print_command_error(command_name);
            print_command_error("copy failed");

            command_end();
            return;
        }

        command_end();
    }
} // namespace shell::commands
