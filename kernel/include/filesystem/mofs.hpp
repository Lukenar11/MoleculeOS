/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file contains the internal implementation of the file system.
    The file system is a flat, inode-based, custom file system residing directly in RAM.

NOTES:
    Since the file system resides directly in RAM, 
    all files—along with their contents—are deleted upon shutting down or restarting the system.
*/

#pragma once

#include "utils/helpers.hpp"
#include "system/panic.hpp"
#include <textmode.hpp>
#include <array.hpp>
#include <string.h>
#include <stdint.h>

namespace kernel::filesystem
{
    class MoleculeOS_File_System final {
    private:
        runtime::Array<Inode, MAX_FILES_PER_DIRECTORY> inodes;

        Inode* allocate_inode() noexcept;

    public:
        bool is_valid_file_name_or_formant_char(const char symbol) const noexcept;

        Inode* get_inode_by_name_and_format(const char* filename, const char* format) 
                                            const noexcept;
        Inode* create_file(const char* filename, const char* format) noexcept;
        void delete_file(Inode* inode) noexcept;

        bool read_file(const Inode* inode,
                       const uint32_t start_byte_index,
                       const uint32_t end_byte_index,
                       const uint32_t buffer_size,
                       uint8_t* out_buffer) noexcept;

        inline const runtime::Array<Inode, MAX_FILES_PER_DIRECTORY>& get_inodes() const noexcept {
            return inodes;
        }

        MoleculeOS_File_System() noexcept = default;
        ~MoleculeOS_File_System() noexcept = default;
    };

    extern MoleculeOS_File_System mofs;
} // namespace kernel::filesystem
