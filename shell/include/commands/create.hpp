#pragma once

#include "utils/helpers.hpp"
#include "kernel/include/filesystem/mofs.hpp"
#include <stdint.h>

namespace shell::commands
{
    void create(const runtime::Array<char, 64>& arguments) noexcept;
} // namespace shell::commands
