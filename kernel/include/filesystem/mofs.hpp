/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:

NOTES:
*/

#pragma once

#include "utils/helpers.hpp"
#include <textmode.hpp>
#include <array.hpp>
#include <string.h>
#include <stdint.h>

namespace kernel::filesystem
{
    class MoleculeOS_File_System final {
    private:
        runtime::Array<Inode, MAX_FILES_PER_DIRECTORY> inodes;

    public:
        Inode* allocate_inode() noexcept;
        Inode* get_inode_by_filename(const char* filename) const noexcept;
        Inode* create_file(char* filename, char* format) noexcept;

        MoleculeOS_File_System() noexcept = default;
        ~MoleculeOS_File_System() noexcept = default;
    };

    extern MoleculeOS_File_System mofs;
} // namespace kernel::filesystem
