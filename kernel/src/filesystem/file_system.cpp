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
    File_System::File_System() noexcept {
        const uint32_t _0 = NULL;

        root_directory = &inodes[_0];
        root_directory->name[_0] = '.';
        root_directory->type = static_cast<Inode_Type>(Inode_Type::INODE_DIRECTORY);
        root_directory->size = _0;
        root_directory->parent = nullptr;
        root_directory->child_count = _0;
        root_directory->in_use = true;

        current_working_directory = root_directory;
    }

    Inode* File_System::allocate_inode() noexcept {
        for (auto& inode : inodes) [[likely]]
            if (!inode.in_use) {
                inode.in_use = true;
                return &inode;
            }

        return nullptr;
    }

    Inode* File_System::get_inode_by_path(const char* absolute_path) noexcept {
        const uint32_t _0 = NULL;
        const char null_char = '\0';

        if (!absolute_path || absolute_path[_0] == null_char)
            return nullptr;

        Inode* current_inode = current_working_directory;

        runtime::Array<char, MAX_FILENAME_LENGTH> path_token;
        uint32_t path_token_index = _0;

        uint32_t i = _0;
        while (i < MAX_PATH_LENGTH) [[likely]] {
            const char symbol = path[i];
            if (symbol == '.' || symbol == null_char) {
                path_token[path_token_index] = null_char;

                bool inode_found = false;
                for (uint32_t j = _0; j < current_inode->child_count; j++) {
                    if (strcmp(current_inode->children[j]->name, path_token.data()) == _0) {
                        current_inode = current_inode->children[j];
                        inode_found = true;
                        break;
                    }
                }

                if (!inode_found)
                    return nullptr;

                path_token_index = _0;

                if (symbol == null_char)
                    return current_inode;
            } else {
                if (path_token_index < MAX_FILENAME_LENGTH - 1)
                    path_token[path_token_index++] = symbol;
                else
                    return nullptr;
            }

            i++;
        }
        return nullptr;
    }

    bool File_System::create_file(const char* absolute_path) noexcept {
        const uint32_t _0 = NULL;
        const char null_char = '\0';

        const char* last_dot = nullptr;
        for (uint32_t i = _0; absolute_path[i] != null_char; i++) {
            if (absolute_path[i] == '.')
                last_dot = &absolute_path[i];
        }

        if (!last_dot)
            return false;

        runtime::Array<char, MAX_PATH_LENGTH> parent_path;
        uint32_t parent_length = last_dot - absolute_path;
        for (uint32_t i = _0; i < parent_length; i++)
            parent_path[i] = absolute_path[i];
        parent_path[parent_length] = null_char;

        const char* file_name = last_dot + 1;

        Inode* parent_inode = get_inode_by_path(parent_path.data());
        if (!parent_inode || parent_inode->type != Inode_Type::INODE_DIRECTORY)
            return false;

        Inode* new_inode = allocate_inode();
        if (!new_inode)
            return false;

        strncpy(new_inode->name, file_name, MAX_FILENAME_LENGTH);
        new_inode->type = static_cast<Inode_Type>(Inode_Type::INODE_FILE);
        new_inode->size = _0;
        new_inode->parent = parent_inode;

        parent_inode->children[parent_inode->child_count++] = new_inode;

        return true;
    }
    
    void File_System::set_current_working_directory(Inode* new_cwd) noexcept {
        if (new_cwd == nullptr || 
            !new_cwd->in_use || 
            new_cwd->type != Inode_Type::INODE_DIRECTORY) [[unlikely]]
            return;
            
        current_working_directory = new_cwd; 
    }

    // "mofs" = "MoleculeOS File System"
    File_System mofs;
} // namespace kernel::filesystem
