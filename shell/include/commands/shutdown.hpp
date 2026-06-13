#pragma once

#include "kernel/include/system/shutdown.hpp"

namespace shell::commands
{
    inline void shutdown() noexcept { 
        kernel::system::shutdown(); 
    }
} // namespace shell::commands
