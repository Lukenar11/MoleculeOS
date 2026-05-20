/*
    LICENSE:
        Copyright (c) 2026 Lukenar11 (Luke Matthes)
        MIT licensed
        https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
    
    DESCRIPTION:
        This is the main shell class that provides the
        high-quality command interface for interacting with the kernel.
    
        The shell reads keyboard input, forwards characters to the command interpreter, 
        and manages the execution flow of user commands.
        
        The shell acts as a central, 
        user-visible system element that coordinates input processing, 
        command parsing, and output through the kernel's text output subsystem.
        
        The shell also provides a cursor and cursor blink for the user.
        
    NOTES:
        The implementation is based on the PS/2 keyboard driver 
        for input and the interpreter class for command processing.
    
        The shell itself contains no parsing logic 
        and serves solely as a control layer and provides a cursor.
*/

#pragma once

#include "drivers/ps2/keyboard_input/include/keyboard_input.hpp"
#include "interpreter/interpreter.hpp"
#include <stddef.h>
#include <stdint.h>
#include <text_output.hpp>

namespace shell
{
    class Shell final {
    private:
        static constexpr uint32_t USER_CURSOR_BLINK_SPEED = 2'500'000;
        
        commands::Interpreter interpreter;

        bool user_cursor_is_visible = true;
        uint32_t user_cursor_blink_counter = NULL;

        void draw_user_cursor() const noexcept;
        void erase_user_cursor() const noexcept;

    public:
        void run() noexcept;

        Shell() noexcept = default;
        ~Shell() noexcept = default;
    };

    // GLOBAL Shell object
    extern Shell shell;
} // namespace shell
