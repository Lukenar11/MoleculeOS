#pragma once

#include "kernel/include/system/reboot.hpp"

namespace shell::commands
{
    inline void reboot() noexcept { 
        kernel::system::reboot(); 
    }
} // namespace shell::commands
