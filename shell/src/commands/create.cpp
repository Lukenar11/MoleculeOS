#include "commands/create.hpp"

namespace shell::commands
{
    void create(const runtime::Array<char, 64>& arguments) noexcept {
        static const char* command_name = "create: ";

        Parsed_File_Name parsed = parse_filename(arguments);
        if (parsed.error.data()[0] != '\0') [[unlikely]] {
            print_command_error(command_name);
            print_command_error(parsed.error.data());

            command_end();
            return;
        }

        auto* inode = kernel::filesystem::mofs.create_file(
            parsed.name.data(), 
            parsed.format.data()
        );
        if (!inode) [[unlikely]] {
            static const char* error_message = "file does already exists";
            print_command_error(command_name);
            print_command_error(error_message);

            command_end();
            return;
        }

        command_end();
    }
} // namespace shell::commands
