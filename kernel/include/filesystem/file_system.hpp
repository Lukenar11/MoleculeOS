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
    class File_System final {
    private:
        runtime::Array<Inode, MAX_FILES_PER_DIRECTORY> inodes;
        Inode* root_directory = nullptr;
        Inode* current_working_directory = nullptr;

        Inode* allocate_inode() noexcept;

    public:
        Inode* get_inode_by_path(const char* absolute_path) noexcept;
        bool create_file(const char* absolute_path) noexcept;

        // bool create_file(const char* file_path, const char* file_name);
        // bool create_directory(const char* dir_path, const char* dir_name);
        // void delete_file(const char* file_path);
        // void delete_directory(const char* dir_path);

        // read_file(const char* path, uint8_t* buffer, uint32_t size);
        // write_file(const char* path, const uint8_t* data, uint32_t size);
        // list_directory(const char* path);

        void set_current_working_directory(Inode* new_cwd) noexcept;

        inline constexpr const Inode* get_current_working_directory() const noexcept { 
            return current_working_directory; 
        }
        
        inline constexpr const Inode* get_current_root_directory() const noexcept { 
            return root_directory; 
        }
        
        File_System() noexcept;
        ~File_System() noexcept = default;
    };

    // "mofs" = "MoleculeOS File System" 
    extern File_System mofs;
} // namespace kernel::filesystem
