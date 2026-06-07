/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This defines the built-in shell commands that are available in
    MoleculeOS. Each command is implemented as a lightweight function
    that performs a specific action.

    These commands are called by the shell interpreter via a
    compile-time generated command list that maps hashed command strings 
    to their corresponding handlers.

NOTES:
    Some shell command implementations are intentionally 
    only implemented in the header, 
    as they are quite small, so that the compiler can inline them.

    Since I want every command to fit into a single function, 
    I'm sometimes forced to write code that isn't entirely clean, 
    such as the "create" function with all its "goto" statements.
*/

#pragma once

#include "kernel/include/system/reboot.hpp"
#include "kernel/include/system/shutdown.hpp"
#include "kernel/include/filesystem/mofs.hpp"
#include "utils/helpers.hpp"
#include <stdint.h>
#include <text_output.hpp>
#include <array.hpp>
#include <string.h>

namespace shell::interpreter
{
    void help() noexcept;
    void info() noexcept;

    inline void clear() noexcept { 
        runtime::text_output.reset(); 
    }

    inline void reboot() noexcept { 
        kernel::system::reboot(); 
    }

    inline void shutdown() noexcept { 
        kernel::system::shutdown(); 
    }

    void echo(const runtime::Array<char, 64>& arguments) noexcept;
    void create(const runtime::Array<char, 64>& arguments) noexcept;
    void list() noexcept;
    void remove(const runtime::Array<char, 64>& arguments) noexcept;
} // namespace shell::interpreter
