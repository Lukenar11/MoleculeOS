/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file contains the internal implementation of the file system.
    The file system is a flat custom file system residing directly in RAM.

NOTES:
    Since the file system resides directly in RAM, 
    all files—along with their contents—are deleted upon 
    shutting down or restarting the sys.

    One method are placed in the header 
    because they are so small that the compiler can inline them.

    Some functions are placed in the header 
    because they are so small that the compiler can inline them.
*/

#pragma once

#include "filesys/helpers.hpp"
#include "heap/block.hpp"
#include <array.hpp>
#include <stdint.h>
#include <string_manip.hpp>

namespace kernel::filesys
{
    class MoleculeOS_File_System_2 final {
    private:
        static inline runtime::Array<I_Node, 256> inode_table;

        static uint32_t to_fnv1a_hash(const char* txt) noexcept;

    public:

        static inline constexpr const runtime::Array<I_Node, 256>& get_inode_table() 
        noexcept {
            return inode_table;
        };

        static I_Node* create_file(const char* name, 
                                   const char* format, 
                                   const uint32_t byte_size) noexcept;

        static bool delete_file(const char* file_name, 
                                const char* file_format) noexcept;

        static bool is_valid_name_or_format_char(const char symbol) noexcept;
    };
} // namespace kernel::filesys
