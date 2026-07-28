#pragma once

#include "../utils/cmds_helpers.hpp"
#include <kernel_api.hpp>

namespace shell::commands
{
    void copy(const runtime::Array<char, 64>& args) noexcept;
} // namespace shell::commands
