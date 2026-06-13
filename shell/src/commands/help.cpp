#include "commands/help.hpp"

namespace shell::commands
{
    void help() noexcept {
        static const char help_message[] = {
            "Command syntax:\n"
            "\t- [Command]\n"
            "\t- [Command] [Argument]\n"
            "\nAvailable commands:\n"
            "\t- help (displays a command-line explanation) | help\n"
            "\t- info (displays the MoleculeOS startup screen) | info\n"
            "\t- clear (clears the screen) | clear\n"
            "\t- reboot (restart the Computer) | reboot\n"
            "\t- shutdown (turn the Computer off) | shutdown\n"
            "\t- echo (displays a message) | echo [Message]\n"
            "\t- create (creates a file) | create [FileName] /\n"
            "\t\t\t\t\t\t\t\tcreate [FileName].[FileFormat]\n"
            "\t- list (lists all files) | list\n"
            "\t- remove (deletes a file) | remove [FileName] /\n"
            "\t\t\t\t\t\t\t\tremove [FileName].[FileFormat]\n"
            "\n"
        };

        runtime::text_output.reset();
        runtime::text_output.put_string(help_message);
    }
} // namespace shell::commands
