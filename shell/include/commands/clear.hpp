#pragma once

#include <text_output.hpp>

namespace shell::commands
{
    inline void clear() noexcept { 
        runtime::text_output.reset(); 
    }
} // namespace shell::commands
