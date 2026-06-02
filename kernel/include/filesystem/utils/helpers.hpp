/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:

NOTES:
*/

#pragma once

#include <stdint.h>

namespace kernel::filesystem
{
    constexpr uint32_t MAX_FILENAME_LENGTH = 16;
    constexpr uint32_t MAX_FILE_SIZE = 4096;
    constexpr uint32_t MAX_FILES_PER_DIRECTORY = 16;
    constexpr uint32_t MAX_PATH_LENGTH = 256;

    enum class Inode_Type : uint8_t {
        INODE_FILE = 0,
        INODE_DIRECTORY = 1
    };

    struct Inode final {
        char name[MAX_FILENAME_LENGTH];
        Inode_Type type;
        uint32_t size;
        uint8_t data[MAX_FILE_SIZE];
        Inode* parent;
        Inode* children[MAX_FILES_PER_DIRECTORY];
        uint32_t child_count;
        bool in_use;
    };
} // namespace kernel::filesystem
