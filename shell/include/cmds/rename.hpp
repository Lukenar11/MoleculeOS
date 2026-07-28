#pragma once

#include "../utils/cmds_helpers.hpp"
#include <array.hpp>
#include <stdint.h>
#include <kernel_api.hpp>

namespace shell::commands
{
    void rename(const runtime::Array<char, 64>& args) noexcept;
} // namespace shell::commands
