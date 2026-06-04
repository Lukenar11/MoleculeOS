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

#include "filesystem/mofs.hpp"

namespace kernel::filesystem
{
    Inode* MoleculeOS_File_System::allocate_inode() noexcept {
        for (auto& inode : inodes) [[likely]]
            if (!inode.in_use) {
                const uint32_t null = 0;
                memset(&inode, null, sizeof(Inode));
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
        const uint32_t null = 0;
        const uint32_t one = 1;
        const char null_terminator = '\0';

        if (!filename || (filename[null] == null_terminator)) return nullptr;
        if (!format || (format[null] == null_terminator)) return nullptr;

        for (auto& inode : inodes) {
            if (!inode.in_use)
                continue;

            if ((strcmp(inode.name, filename) == null) &&
                (strcmp(inode.format, format) == null))
                return nullptr;
        }

        Inode* inode = allocate_inode();
        if (!inode)
            return nullptr;

        strncpy(inode->name, filename, MAX_FILENAME_LENGTH - one);
        strncpy(inode->format, format, MAX_FILE_FORMAT_NAME_LENGTH - one);
        inode->name[MAX_FILENAME_LENGTH - one] = null_terminator;
        inode->format[MAX_FILE_FORMAT_NAME_LENGTH - one] = null_terminator;

        inode->size = null;

        return inode;
    }

    MoleculeOS_File_System mofs;
} // namespace kernel::filesystem
