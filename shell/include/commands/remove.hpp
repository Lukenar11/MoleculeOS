#pragma once

#include "utils/helpers.hpp"
#include "kernel/include/filesystem/mofs.hpp"
#include <array.hpp>
#include <text_output.hpp>

namespace shell::commands
{
    void remove(const runtime::Array<char, 64>& arguments) noexcept;
} // namespace shell::commands
