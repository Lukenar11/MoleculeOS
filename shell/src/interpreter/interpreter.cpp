/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This is the command interpreter used by the MoleculeOS shell. 
            
    The interpreter receives raw keyboard input, 
    tokenizes the input string, separates command and arguments,
    and dispatches the appropriate handler based on a compile‑time hash lookup table.
    
NOTES:
    The "make_hash" function and the "shell_command_table" 
    are intentionally declared only in the header 
    to ensure that the hash lookup table can be created at compile time.

    Since the functions "set_error_message_text_color" and 
    "set_error_message_text_color" are so short, 
    I put them in the header so the compiler can inline them.

    The method "append_char" is only in the header because it uses a template, 
    and templates must be inline in the header.
*/

#include "interpreter.hpp"

namespace shell::commands
{
    void Interpreter::print_overflow_error(const char* error_message_for, 
                                           const uint32_t max_buffer_size) const noexcept {
        static const char* error_messages[] = {
            "SHELL ERROR:\n\t",
            " overflow. Max size: ",
            " symbols\n\n"
        };

        set_error_message_text_color();

        runtime::text_output.put_string(error_messages[0]);
        runtime::text_output.put_string(error_message_for);
        runtime::text_output.put_string(error_messages[1]);
        runtime::text_output.put_uint(max_buffer_size);
        runtime::text_output.put_string(error_messages[2]);

        set_default_text_color();
    };

    bool Interpreter::tokenize_input_buffer() {
        bool tokenize_commands = true;
        for (uint32_t i = NULL; i < input_buffer_index; ++i) [[likely]] {
            const char key = input_buffer[i];

            if (key == NULL_TERMINATOR) [[unlikely]]
                break;

            if (tokenize_commands && commands_index == NULL && key == ' ')
                continue;

            if (key == ' ' && arguments_index == NULL) {
                tokenize_commands = false;
                continue;
            }

            auto& target_buffer = (tokenize_commands) ? commands : arguments;
            auto& target_index = (tokenize_commands) ? commands_index : arguments_index;
            if (!append_char(target_buffer, target_index, key)) [[unlikely]] {
                static const char* command_buffer_error_message = "Command buffer";
                static const char* argument_buffer_error_message = "Argument buffer";

                const char* overflow_error_message = (tokenize_commands) 
                                                     ? command_buffer_error_message
                                                     : argument_buffer_error_message;
                    
                print_overflow_error(overflow_error_message, target_buffer.size());
                return false;
            }
        }

        commands[commands_index] = NULL_TERMINATOR;
        arguments[arguments_index] = NULL_TERMINATOR;

        return true;
    }

    bool Interpreter::validate_tokens() noexcept {
        if (commands_index == NULL && arguments_index > NULL) [[unlikely]] {
            set_error_message_text_color();

            static const char* error_message = "Error: arguments without command.\n\n";
            runtime::text_output.put_string(error_message);
            
            set_default_text_color();

            return false;
        }

        if (commands_index == NULL || commands[NULL] == NULL_TERMINATOR) [[unlikely]] {
            static const char* error_message = "No command entered.\n\n";
            runtime::text_output.put_string(error_message);

            return false;
        }

        return true;
    }

    void Interpreter::parse_commands() noexcept {
        const uint32_t command_hash = make_hash(commands.data());
        for (const auto& entry : shell_command_table) [[likely]]
            if (entry.hash == command_hash) {
                entry.function(arguments);
                return;
            }

        static const char error_messages[] = {
            "COMMAND ERROR:\n"
            "\tThe command was either misspelled or does not exist!\n"
            "\tType \"help\" to see a list of commands.\n\n"
        };

        set_error_message_text_color();
        runtime::text_output.put_string(error_messages);
        set_default_text_color();
    }

    constexpr void Interpreter::flush_interpreter_pipeline() noexcept {
        const char null_terminator = NULL_TERMINATOR;
        input_buffer.fill(null_terminator);
        commands.fill(null_terminator);
        arguments.fill(null_terminator);

        input_buffer_index = NULL;
        commands_index = NULL;
        arguments_index = NULL;
    }

    void Interpreter::step(const char& key) {
        if (key == '\b') {
            if (input_buffer_index > NULL) {
                --input_buffer_index;
                input_buffer[input_buffer_index] = NULL_TERMINATOR;
            }
            return;
        }

        if (key == '\n') [[unlikely]] {
            const bool is_tokenized = tokenize_input_buffer();
            if (is_tokenized && validate_tokens())
                parse_commands();

            flush_interpreter_pipeline();
            return;
        }

        if (append_char(input_buffer, input_buffer_index, key))
            return;

        static const char* error_message = "Input buffer";
        runtime::text_output.put_char('\n');
        print_overflow_error(error_message, input_buffer.size());

        flush_interpreter_pipeline();
    }
} // namespace shell::commands
