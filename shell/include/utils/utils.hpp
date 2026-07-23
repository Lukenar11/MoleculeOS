/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This helper function takes an array, an index, and a value, 
    and replaces the array element at the specified index with the passed value; 
    it also returns a boolean indicating whether the 
    operation was successful or not.
    
NOTES:
    Since this function is fairly small, 
    it's included in the header so the compiler can inline it.
*/

#pragma once

#include <stdint.h>
#include <array.hpp>

namespace shell
{
    template <typename Arr>
    inline constexpr bool append_char(Arr& buffer, 
                                      uint32_t& index,
                                      const char symbol) noexcept {
        if (index < buffer.size()) [[likely]] {
            buffer[index++] = symbol;
            return true;
        }
        return false;
    }
} // namespace shell
