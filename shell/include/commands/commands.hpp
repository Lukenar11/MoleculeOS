/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This defines the built-in shell commands that are available in MoleculeOS. 
    Each command is implemented as a function
    that performs a specific action.

    These commands are called by the shell interpreter via a
    compile-time generated command list that maps hashed command strings 
    to their corresponding handlers.

NOTES:
    Some shell command implementations are intentionally 
    only implemented in the header, 
    as they are quite small, so that the compiler can inline them.
*/

#pragma once

#include "kernel/include/system/reboot.hpp"
#include "kernel/include/system/shutdown.hpp"
#include <text_output.hpp>
#include "create.hpp"
#include "echo.hpp"
#include "help.hpp"
#include "info.hpp"
#include "list.hpp"
#include "remove.hpp"

namespace shell::commands
{
    inline void clear() noexcept { 
        runtime::text_output.reset(); 
    }

    inline void reboot() noexcept { 
        kernel::system::reboot(); 
    }

    inline void shutdown() noexcept { 
        kernel::system::shutdown(); 
    }
} // namespace shell::commands
