/**
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

#pragma once

#include "cmds.hpp"
#include "utils/shell_helpers.hpp"
#include "utils/utils.hpp"
#include <types.hpp>
#include <array.hpp>
#include <text_output.hpp>

namespace
{
    [[nodiscard]]
    inline constexpr uint32_t make_hash(const char* command) noexcept {
        const uint32_t mul_32 = 5;
        uint32_t hash         = 0;
        while (*command) [[likely]] {
            hash = (hash << mul_32) - hash + static_cast<char>(*command);
            command++;
        }

        return hash;
    }

    constexpr shell::Command_Entry shell_command_table[] = {
        {make_hash("help"),      [](auto& _)    -> void {shell::commands::help();}},
        {make_hash("info"),      [](auto& _)    -> void {shell::commands::info();}},
        {make_hash("clear"),     [](auto& _)    -> void {shell::commands::clear();}},
        {make_hash("reboot"),    [](auto& _)    -> void {shell::commands::reboot();}},
        {make_hash("shutdown"),  [](auto& _)    -> void {shell::commands::shutdown();}},
        {make_hash("echo"),      [](auto& args) -> void {shell::commands::echo(args);}},
        {make_hash("create"),    [](auto& args) -> void {shell::commands::create(args);}},
        {make_hash("list"),      [](auto& _)    -> void {shell::commands::list();}},
        {make_hash("remove"),    [](auto& args) -> void {shell::commands::remove(args);}},
        {make_hash("rename"),    [](auto& args) -> void {shell::commands::rename(args);}},
        {make_hash("copy"),      [](auto& args) -> void {shell::commands::copy(args);}},
        {make_hash("exists"),    [](auto& args) -> void {shell::commands::exists(args);}},
        {make_hash("allocinfo"), [](auto& args) -> void {shell::commands::allocinfo(args);}}
    };
}

namespace shell
{
    class Shell final {
    private:
        runtime::Array<char, 128> input_buffer;
        runtime::Array<char, 64> commands;
        runtime::Array<char, 64> args;

        uint32_t input_buffer_index = 0;
        uint32_t commands_index = 0;
        uint32_t args_index = 0;

        static inline void set_error_message_text_color() noexcept {
            runtime::Text_Output::set_text_color(drivers::vga::Text_Mode_Colors::LIGHT_MAGENTA,
                                                 drivers::vga::Text_Mode_Colors::BLACK);
        }

        static inline void set_default_text_color() noexcept {
            runtime::Text_Output::set_text_color(drivers::vga::Text_Mode_Colors::LIGHT_GREY,
                                                 drivers::vga::Text_Mode_Colors::BLACK);
        }

        void print_overflow_error(const char* error_message_for, 
                                  const uint32_t max_buffer_size) const noexcept;

        bool validate_tokens() noexcept;

        bool tokenize_input_buffer();
        void parse_commands() noexcept;
            
        constexpr void flush_pipeline() noexcept;

    public:
        void step(const char& key);

        Shell() noexcept  = default;
        ~Shell() noexcept = default;
    };
} // namespace shell
