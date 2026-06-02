/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:

NOTES:
*/

#include "filesystem/file_system.hpp"

namespace kernel::filesystem
{
    File_System::File_System() {
        for (auto& inode : inodes) [[likely]]
            inode.in_use = false;

        root_directory = &inodes[NULL];
        root_directory->name[NULL] = '.';
        root_directory->name[NULL] = '\n';
        root_directory->type = static_cast<Inode_Type>(Inode_Type::INODE_DIRECTORY);
        root_directory->size = NULL;
        root_directory->parent = nullptr;
        root_directory->child_count = NULL;
        root_directory->in_use = true;

        current_working_directory = root_directory;
    }
    
    // "mofs" = "MoleculeOS File System" 
    File_System mofs;
} // namespace kernel::filesystem
