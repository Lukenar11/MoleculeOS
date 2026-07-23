/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This is the command interpreter used by the MoleculeOS shell. 
        
    The interpreter receives raw keyboard input, 
    tokenizes the input string, separates command and arguments,
    and dispatches the 
    appropriate handler based on a compile‑time hash lookup table.
    
NOTES:
    Some methods are placed in the header 
    because they are so small that the compiler can inline them.
*/

#include "shell.hpp"

namespace shell
{
    void Shell::print_overflow_error(const char* error_message_for, 
                                     const uint32_t max_buffer_size)
                                     const noexcept {
        static const char* error_messages[] = {
            "SHELL ERROR:\n\t",
            " overflow. Max size: ",
            " symbols\n\n"
        };

        set_error_message_text_color();

        runtime::Text_Output::put_string(error_messages[0]);
        runtime::Text_Output::put_string(error_message_for);
        runtime::Text_Output::put_string(error_messages[1]);
        runtime::Text_Output::put_uint(max_buffer_size);
        runtime::Text_Output::put_string(error_messages[2]);

        set_default_text_color();
    };

    bool Shell::tokenize_input_buffer() {
        bool tokenize_commands = true;
        for (uint32_t i = 0; i < input_buffer_index; ++i) [[likely]] {
            const char key = input_buffer[i];

            if (key == '\0') [[unlikely]]
                break;

            if (tokenize_commands && commands_index == 0 && key == ' ')
                continue;

            if (key == ' ' && args_index == 0) {
                tokenize_commands = false;
                continue;
            }

            auto& target_buffer = (tokenize_commands) ? commands : args;
            auto& target_index  = (tokenize_commands) ? commands_index : args_index;
            if (!append_char(target_buffer, target_index, key)) [[unlikely]] {
                static const char* command_buffer_error_message  = "Command buffer";
                static const char* argument_buffer_error_message = "Argument buffer";

                const char* overflow_error_message = (tokenize_commands) 
                                                     ? command_buffer_error_message
                                                     : argument_buffer_error_message;
                    
                print_overflow_error(overflow_error_message, target_buffer.size());
                return false;
            }
        }

        commands[commands_index]   = '\0';
        args[args_index] = '\0';

        return true;
    }

    bool Shell::validate_tokens() noexcept {
        if (commands_index == 0 && args_index > 0) [[unlikely]] {
            set_error_message_text_color();

            static const char* error_message = "Error: args without command.\n\n";
            runtime::Text_Output::put_string(error_message);
        
            set_default_text_color();
            return false;
        }

        if (commands_index == 0 || commands[0] == '\0') [[unlikely]] {
            static const char* error_message = "No command entered.\n\n";
            runtime::Text_Output::put_string(error_message);

            return false;
        }

        return true;
    }

    void Shell::parse_commands() noexcept {
        const uint32_t command_hash = make_hash(commands.data());
        for (const auto& entry : shell_command_table) [[likely]]
            if (entry.hash == command_hash) {
                entry.function(args);
                return;
            }

        static const char error_messages[] = {
            "COMMAND ERROR:\n"
            "\tThe command was either misspelled or does not exist!\n"
            "\tType \"help\" to see a list of commands.\n\n"
        };

        set_error_message_text_color();
        runtime::Text_Output::put_string(error_messages);
        set_default_text_color();
    }

    constexpr void Shell::flush_shell_pipeline() noexcept {
        input_buffer.fill('\0');
        commands.fill('\0');
        args.fill('\0');

        input_buffer_index = 0;
        commands_index     = 0;
        args_index    = 0;
    }

    void Shell::step(const char& key) {
        if (key == '\b') {
            if (input_buffer_index > 0) {
                --input_buffer_index;
                input_buffer[input_buffer_index] = '\0';
            }
            return;
        }

        if (key == '\n') [[unlikely]] {
            const bool is_tokenized = tokenize_input_buffer();
            if (is_tokenized && validate_tokens())
                parse_commands();

            flush_shell_pipeline();
            return;
        }

        if (append_char(input_buffer, input_buffer_index, key))
            return;

        runtime::Text_Output::put_char('\n');
        print_overflow_error("Input buffer", input_buffer.size());

        flush_shell_pipeline();
    }
} // namespace shell
