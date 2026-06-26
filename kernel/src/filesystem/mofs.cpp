/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file contains the internal implementation of the file system.
    The file system is a flat, inode-based, 
    custom file system residing directly in RAM.

NOTES:
    Since the file system resides directly in RAM, 
    all files—along with their contents—are deleted upon shutting down or 
    restarting the system.

    1 method are placed in the header 
    because they are so small that the compiler can inline them.

    Some functions are placed in the header 
    because they are so small that the compiler can inline them.
*/

#include "filesystem/mofs.hpp"

namespace kernel::filesystem
{
    [[nodiscard]]
    Inode* MoleculeOS_File_System::allocate_inode() noexcept {
        for (auto& inode : inodes)
            if (!inode.in_use) {
                runtime::memory_manip.set_memory_block(&inode, 0, sizeof(Inode));
                inode.in_use = true;

                return &inode;
            }

        system::panic(
            "MOFS: no free inodes available", 
            "dont create to mutch files"
        );

        return nullptr;
    }

    void MoleculeOS_File_System::recalculate_file_size(Inode* inode) noexcept {
        if (inode_not_available(inode)) [[unlikely]]
            return;

        int32_t i = static_cast<int32_t>(MAX_FILE_SIZE) - 1;
        for (; i >= 0; --i)
            if (inode->data[i] != 0) {
                inode->size = static_cast<uint32_t>(i + 1);
                return;
            }

        inode->size = 0;
    }

    bool MoleculeOS_File_System::is_valid_file_name_or_formant_char(const char symbol)
                                                                    const noexcept {
        if (((symbol >= 'A') && (symbol <= 'Z')) ||
            ((symbol >= 'a') && (symbol <= 'z')) ||
            ((symbol >= '0') && (symbol <= '9')) ||
            (symbol == '_') || (symbol == '-')) [[likely]]
            return true;

        return false;
    }

    Inode* MoleculeOS_File_System::get_inode_by_name_and_format(const char* filename, 
                                                                const char* format) 
                                                                const noexcept {
        for (auto& inode : inodes) {
            if (!inode.in_use)
                continue;

            if ((runtime::string_manip.compare_strings(inode.name, filename) == 0) &&
                (runtime::string_manip.compare_strings(inode.format, format) == 0))
                return const_cast<Inode*>(&inode);
        }

        return nullptr;
    }

    Inode* MoleculeOS_File_System::create_file(const char* filename, 
                                               const char* format) noexcept {
        if (!filename || (filename[0] == '\0')) [[unlikely]] 
            return nullptr;

        for (uint32_t i = 0; filename[i] != '\0'; i++)
            if (!is_valid_file_name_or_formant_char(filename[i])) [[unlikely]]
                return nullptr;

        for (uint32_t i = 0; format[i] != '\0'; i++)
            if (!is_valid_file_name_or_formant_char(format[i])) [[unlikely]]
                return nullptr;

        for (auto& inode : inodes) {
            if (!inode.in_use)
                continue;

            if ((runtime::string_manip.compare_strings(inode.name, filename) == 0) &&
                (runtime::string_manip.compare_strings(inode.format, format) == 0)) [[unlikely]]
                return nullptr;
        }

        Inode* inode = allocate_inode();
        if (!inode) [[unlikely]]
            return nullptr;

        runtime::string_manip.copy_string_part(
            inode->name, 
            filename, 
            MAX_FILENAME_LENGTH
        );
        
        runtime::string_manip.copy_string_part(
            inode->format, 
            format, 
            MAX_FILE_FORMAT_NAME_LENGTH
        );

        inode->name[MAX_FILENAME_LENGTH - 1] = '\0';
        inode->format[MAX_FILE_FORMAT_NAME_LENGTH - 1] = '\0';
        inode->size = 0;

        return inode;
    }

    void MoleculeOS_File_System::delete_file(Inode* inode) noexcept {
        inode->in_use = false;
        inode->size = 0;

        runtime::memory_manip.set_memory_block(
            inode->name, 
            '\0', 
            MAX_FILENAME_LENGTH
        );

        runtime::memory_manip.set_memory_block(
            inode->format, 
            '\0', 
            MAX_FILE_FORMAT_NAME_LENGTH
        );

        runtime::memory_manip.set_memory_block(
            inode->data, 
            0, 
            MAX_FILE_SIZE
        );
    }

    bool MoleculeOS_File_System::get_file_content_binary(const Inode* inode, 
                                                         uint8_t* out_buffer,
                                                         const uint32_t buffer_size)
                                                         noexcept {
        if (inode_not_available(inode)) [[unlikely]] 
            return false;

        if (less_then(buffer_size, inode->size)) [[unlikely]] 
            return false;

        runtime::memory_manip.copy_memory_block(
            out_buffer, 
            inode->data, 
            inode->size
        );
        return true;
    }

    bool MoleculeOS_File_System::get_file_content_as_string(const Inode* inode, 
                                                            char* out_buffer,
                                                            const uint32_t buffer_size)
                                                            noexcept {
        if (inode_not_available(inode)) [[unlikely]] 
            return false;

        if (less_then(buffer_size, inode->size + 1)) [[unlikely]] 
            return false;

        runtime::memory_manip.copy_memory_block(
            out_buffer, 
            inode->data, 
            inode->size
        );
        out_buffer[inode->size] = '\0';

        return true;
    }

    bool MoleculeOS_File_System::read_file_binary_at(const Inode* inode,
                                                     uint8_t* out_buffer,
                                                     const uint32_t buffer_size,
                                                     const uint32_t offset,
                                                     const uint32_t length) 
                                                     noexcept {
        if (read_file_at_guard(inode, offset, length)) [[unlikely]] 
            return false;

        if (less_then(buffer_size, length)) [[unlikely]] 
            return false;

        runtime::memory_manip.copy_memory_block(
            out_buffer, 
            (inode->data + offset), 
            length
        );

        return true;
    }

    bool MoleculeOS_File_System::read_file_as_string_at(const Inode* inode, 
                                                        char* out_buffer,
                                                        const uint32_t buffer_size,
                                                        const uint32_t offset, 
                                                        const uint32_t length) 
                                                        noexcept {
        if (read_file_at_guard(inode, offset, length)) [[unlikely]] 
            return false;

        if (less_then(buffer_size, (length + 1))) [[unlikely]] 
            return false;

        runtime::memory_manip.copy_memory_block(
            out_buffer, 
            (inode->data + offset), 
            length
        );
        out_buffer[length] = '\0';

        return true;
    }

    bool MoleculeOS_File_System::set_file_content_binary(Inode* inode,
                                                         const uint8_t* in_buffer,
                                                         const uint32_t length)
                                                         noexcept {
        if (set_file_content_guard(inode, length)) [[unlikely]]
            return false;

        runtime::memory_manip.copy_memory_block(
            inode->data, 
            in_buffer, 
            length
        );
        recalculate_file_size(inode);

        return true;
    }

    bool MoleculeOS_File_System::set_file_content_as_string(Inode* inode,
                                                            const char* in_buffer,
                                                            const uint32_t length) 
                                                            noexcept {
        if (set_file_content_guard(inode, length)) [[unlikely]]
            return false;

        runtime::memory_manip.copy_memory_block(
            inode->data, 
            in_buffer, 
            length
        );
        recalculate_file_size(inode);

        return true;
    }

    bool MoleculeOS_File_System::write_file_binary_at(Inode* inode,
                                                      const uint8_t* in_buffer,
                                                      const uint32_t buffer_size,
                                                      const uint32_t offset,
                                                      const uint32_t length)
                                                      noexcept {
        if (write_fill_at_guard(inode, offset, length, buffer_size)) [[unlikely]]
            return false;

        runtime::memory_manip.copy_memory_block(
            (inode->data + offset), 
            in_buffer, 
            length
        );
        recalculate_file_size(inode);

        return true;
    }

    bool MoleculeOS_File_System::write_file_string_at(Inode* inode,
                                                      const char* in_buffer,
                                                      const uint32_t buffer_size,
                                                      const uint32_t offset,
                                                      const uint32_t length) 
                                                      noexcept {
        if (write_fill_at_guard(inode, offset, length, buffer_size)) [[unlikely]]
            return false;

        runtime::memory_manip.copy_memory_block(
            (inode->data + offset), 
            in_buffer, 
            length
        );
        recalculate_file_size(inode);

        return true;
    }

    MoleculeOS_File_System mofs;
} // namespace kernel::filesystem
