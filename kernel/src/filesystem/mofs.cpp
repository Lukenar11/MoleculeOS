/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:

NOTES:
*/

#include "filesystem/mofs.hpp"

namespace kernel::filesystem
{
    Inode* MoleculeOS_File_System::allocate_inode() noexcept {
        for (auto& inode : inodes) [[likely]]
            if (!inode.in_use) {
                inode.in_use = true;
                return &inode;
            }
            
        return nullptr;
    }

    Inode* MoleculeOS_File_System::get_inode_by_filename(const char* filename) const noexcept {
        const uint32_t null = 0;
        for (auto& inode : inodes) [[likely]]
            if (inode.in_use && (strcmp(inode.name, filename) == null))
                return const_cast<Inode*>(&inode);

        return nullptr;        
    }

    Inode* MoleculeOS_File_System::create_file(const char* filename, const char* format) noexcept {
        Inode* current_inode = allocate_inode();
        if (!current_inode) [[unlikely]]
            return nullptr;

        strncpy(current_inode->name, filename, MAX_FILE_SIZE);
        strncpy(current_inode->format, format, MAX_FILE_FORMAT_NAME_LENGTH);

        current_inode->in_use = true;
        current_inode->size = 0;

        return current_inode;
    }

    MoleculeOS_File_System mofs;
} // namespace kernel::filesystem
