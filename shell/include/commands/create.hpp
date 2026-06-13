#pragma once

#include "utils/helpers.hpp"
#include "kernel/include/filesystem/mofs.hpp"
#include <array.hpp>

namespace shell::commands
{
    void create(const runtime::Array<char, 64>& arguments) noexcept;
} // namespace shell::commands
