#include "commands/remove.hpp"

namespace shell::commands
{
    void remove(const runtime::Array<char, 64>& arguments) noexcept {
        static const char* command_name = "remove: ";

        Parsed_File_Name parsed = parse_filename(arguments);
        if (parsed.error.data()[0] != '\0') [[unlikely]] {
            print_command_error(command_name);
            print_command_error(parsed.error.data());

            command_end();
            return;
        }

        auto* inode = kernel::filesystem::mofs.get_inode_by_name_and_format(
            parsed.name.data(),
            parsed.format.data()
        );

        if (!inode) [[unlikely]] {
            static const char* error_message = "file does not exist";
            print_command_error(command_name);
            print_command_error(error_message);

            command_end();
            return;
        }

        kernel::filesystem::mofs.delete_file(inode);

        command_end();
    }
} // namespace shell::commands
