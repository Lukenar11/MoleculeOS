/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
NOTES:
*/

#pragma once

#include <array.hpp>
#include <stdint.h>

namespace kernel::filesys
{
    constexpr uint32_t MAX_FILE_NAME_LENGTH   = 8;
    constexpr uint32_t MAX_FILE_FORMAT_LENGTH = 4;

    struct I_Node final {
        runtime::Array<char, MAX_FILE_NAME_LENGTH + 1> file_name;
        runtime::Array<char, MAX_FILE_FORMAT_LENGTH + 1> file_format;
        uint32_t name_hash      = 0;
        uint32_t format_hash    = 0;
        uint32_t file_byte_size = 0;
        void* file_data_ptr     = 0;
    };
} // namespace kernel::filesys
