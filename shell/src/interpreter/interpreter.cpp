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

        The method "append_char" is only in the header because it uses a template, 
        and templates must be inline in the header.
*/

#include "interpreter.hpp"

namespace shell::commands
{
    void Interpreter::print_overflow_error(const char* error_message_for, 
                                           uint32_t max_buffer_size) const noexcept {
        static const char* error_messages[] = {
            "\nSHELL ERROR:\n\t",
            " overflow. Max size: ",
            " symbols\n\n"
        };

        runtime::text_output.put_string(error_messages[0]);
        runtime::text_output.put_string(error_message_for);
        runtime::text_output.put_string(error_messages[1]);
        runtime::text_output.put_uint(max_buffer_size);
        runtime::text_output.put_string(error_messages[2]);
    };

    bool Interpreter::tokenize_input_buffer() {
        bool tokenize_commands = true;
        for (uint32_t i = 0; i < input_buffer_index; ++i) {
            const char key = input_buffer[i];

            if (key == '\0')
                break;

            if (tokenize_commands && commands_index == 0 && key == ' ')
                continue;

            if (key == ' ' && arguments_index == 0) {
                tokenize_commands = false;
                continue;
            }

            auto& target_buffer = (tokenize_commands) ? commands : arguments;
            auto& target_index = (tokenize_commands) ? commands_index : arguments_index;
            if (!append_char(target_buffer, target_index, key)) {
                static const char* command_buffer_error_message = "Command buffer";
                static const char* argument_buffer_error_message = "Argument buffer";

                const char* overflow_error_message = (tokenize_commands) 
                                                     ? command_buffer_error_message
                                                     : argument_buffer_error_message;
                    
                print_overflow_error(overflow_error_message, target_buffer.size());
                return false;
            }
        }

        commands[commands_index] = '\0';
        arguments[arguments_index] = '\0';

        return true;
    }

    bool Interpreter::validate_tokens() const noexcept {
        if (commands_index == 0 && arguments_index > 0) {
            static const char* error_message = "Error: arguments without command.\n\n";
            runtime::text_output.put_string(error_message);

            return false;
        }

        if (commands_index == 0 || commands[0] == '\0') {
            static const char* error_message = "No command entered.\n\n";
            runtime::text_output.put_string(error_message);

            return false;
        }

        return true;
    }

    void Interpreter::parse_commands() noexcept {
        const uint32_t command_hash = make_hash(commands.data());
        for (const auto& entry : shell_command_table)
            if (entry.hash == command_hash) {
                entry.function(arguments);
                return;
            }

        static const char error_messages[] = {
            "COMMAND ERROR:\n"
            "\tThe command was either misspelled or does not exist!\n"
            "\tType \'help\' to see a list of commands.\n\n"
        };
        
        runtime::text_output.put_string(error_messages);
    }

    constexpr void Interpreter::flush_interpreter_pipeline() noexcept {
        const char nullterminator = '\0';
        const uint32_t null = 0;

        input_buffer.fill(nullterminator);
        commands.fill(nullterminator);
        arguments.fill(nullterminator);

        input_buffer_index = null;
        commands_index = null;
        arguments_index = null;
    }

    void Interpreter::step(const char& key) {
        if (key == '\b') {
            if (input_buffer_index > 0) {
                --input_buffer_index;
                input_buffer[input_buffer_index] = '\0';
            }
            return;
        }

        if (key == '\n') {
            const bool is_tokenized = tokenize_input_buffer();
            if (is_tokenized && validate_tokens())
                parse_commands();

            flush_interpreter_pipeline();
            return;
        }

        if (append_char(input_buffer, input_buffer_index, key))
            return;

        static const char* error_message = "Input buffer";
        print_overflow_error(error_message, input_buffer.size());
        flush_interpreter_pipeline();
    }
} // namespace shell::commands
