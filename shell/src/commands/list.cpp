#include "commands/list.hpp"

namespace shell::commands
{
    void list() noexcept {
        runtime::text_output.reset();
        runtime::text_output.put_string("Files:\n");

        const auto& inodes = kernel::filesystem::mofs.get_inodes();
        for (const auto& inode : inodes) [[likely]] {
            if (!inode.in_use)
                continue;

            if (inode.name[0] == '\0') {
                static const char* error_message = "\t- [INVALID INODE]\n";
                runtime::text_output.put_string(error_message);
                continue;
            }

            runtime::text_output.put_string("\t- ");
            runtime::text_output.put_string(inode.name);

            if (inode.format[0] != '\0') {
                runtime::text_output.put_char('.');
                runtime::text_output.put_string(inode.format);
            }

            runtime::text_output.put_char('\n');
        }

        runtime::text_output.put_char('\n');
    }
} // namespace shell::commands
