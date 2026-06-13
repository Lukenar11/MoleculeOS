#include "commands/echo.hpp"

namespace shell::commands
{
    void echo(const runtime::Array<char, 64>& arguments) noexcept {
        const char null_terminator = '\0';

        if (arguments[0] == null_terminator) [[unlikely]] {
            static const char* error_message = "echo: missing argument\n\n";
            print_command_error(error_message);
            return;
        }

        for (const auto& symbol : arguments) [[likely]] {
            if (symbol == null_terminator) [[unlikely]]
                break;
            
            runtime::text_output.put_char(symbol);
        }

        command_end();
    }
} // namespace shell::commands
