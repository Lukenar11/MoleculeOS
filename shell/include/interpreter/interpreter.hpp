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
*/

#pragma once

#include "commands/commands.hpp"
#include "utils/command_entry.hpp"
#include "utils/append_char.hpp"
#include <stdint.h>
#include <array.hpp>
#include <text_output.hpp>

namespace
{
    [[nodiscard]]
    inline constexpr uint32_t make_hash(const char* command) noexcept {
        const uint32_t mul_32 = 5;
        uint32_t hash = 0;
        while (*command) [[likely]] {
            hash = (hash << mul_32) - hash + static_cast<char>(*command);
            command++;
        }
        return hash;
    }

    constexpr shell::interpreter::Command_Entry shell_command_table[] = {
        {make_hash("help"), [](auto& _) -> void {shell::commands::help();}},
        {make_hash("info"), [](auto& _) -> void {shell::commands::info();}},
        {make_hash("clear"), [](auto& _) -> void {shell::commands::clear();}},
        {make_hash("reboot"), [](auto& _) -> void {shell::commands::reboot();}},
        {make_hash("shutdown"), [](auto& _) -> void {shell::commands::shutdown();}},
        {make_hash("echo"), [](auto& arguments) -> void {shell::commands::echo(arguments);}},
        {make_hash("create"), [](auto& arguments) -> void {shell::commands::create(arguments);}},
        {make_hash("list"), [](auto& _) -> void {shell::commands::list();}},
        {make_hash("remove"), [](auto& arguments) -> void {shell::commands::remove(arguments);}}
    };
}

namespace shell::interpreter
{
    class Interpreter final {
    private:
        static constexpr char NULL_TERMINATOR = '\0';

        runtime::Array<char, 128> input_buffer;
        runtime::Array<char, 64> commands;
        runtime::Array<char, 64> arguments;

        uint32_t input_buffer_index = 0;
        uint32_t commands_index = 0;
        uint32_t arguments_index = 0;

        static inline void set_error_message_text_color() noexcept {
            runtime::text_output.set_text_color(
                drivers::vga::VGA_Textmode_Colors::LIGHT_MAGENTA,
                drivers::vga::VGA_Textmode_Colors::BLACK
            );
        }

        static inline void set_default_text_color() noexcept {
            runtime::text_output.set_text_color(
                drivers::vga::VGA_Textmode_Colors::LIGHT_GREY,
                drivers::vga::VGA_Textmode_Colors::BLACK
            );
        }

        template <typename Arr>
        inline constexpr bool append_char(Arr& buffer, 
                                          uint32_t& index,
                                          const char symbol) noexcept {
            if (index < buffer.size()) [[likely]] {
                buffer[index++] = symbol;
                return true;
            }
            return false;
        }

        void print_overflow_error(const char* error_message_for, 
                                  const uint32_t max_buffer_size) const noexcept;

        bool validate_tokens() noexcept;

        bool tokenize_input_buffer();
        void parse_commands() noexcept;
            
        constexpr void flush_interpreter_pipeline() noexcept;

    public:
        void step(const char& key);

        Interpreter() noexcept = default;
        ~Interpreter() noexcept = default;
    };
} // namespace shell::interpreter
