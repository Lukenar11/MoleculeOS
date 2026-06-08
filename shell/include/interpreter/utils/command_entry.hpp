/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION: 
    This file defines the CommandEntry structure used by the shell interpreter 
    to associate hashed command strings with their corresponding handler functions. 
        
    Each entry contains a precomputed 32‑bit hash and a 
    function pointer that receives the parsed argument buffer.
     
NOTES: 
    The function pointer signature is designed to accept a 
    fixed-size "runtime::Array<char, 64>" 
    containing the parsed argument string, this avoids dynamic memory usage.
*/

#pragma once

#include <stdint.h>
#include <array.hpp>

namespace shell::interpreter
{
    struct Command_Entry final {
        uint32_t hash;
        void (*function)(const runtime::Array<char, 64>& argumentes);
    };
} // namespace shell::interpreter
