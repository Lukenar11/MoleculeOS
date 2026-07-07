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
    all files—along with their contents—are deleted upon 
    shutting down or restarting the system.

    One method are placed in the header 
    because they are so small that the compiler can inline them.

    Some functions are placed in the header 
    because they are so small that the compiler can inline them.
*/

#pragma once

#include "utils/helpers.hpp"
#include <array.hpp>
#include <kernel_arch_api.hpp>
#include <memory_manipulation.hpp>
#include <string_manipulation.hpp>
#include <stdint.h>

namespace
{
    [[nodiscard]]
    inline constexpr bool inode_not_available(const kernel::filesystem::Inode* inode)
                                              noexcept {
        return !inode || !inode->in_use; 
    }

    [[nodiscard]]
    inline constexpr bool less_then(const uint32_t a, 
                                    const uint32_t b) noexcept {
        return a < b; 
    }

    [[nodiscard]]
    inline constexpr bool greader_then(const uint32_t a, 
                                       const uint32_t b) noexcept {
        return a > b; 
    }

    [[nodiscard]]
    inline constexpr bool read_file_at_guard(const kernel::filesystem::Inode* inode,
                                             const uint32_t offset, 
                                             const uint32_t length) noexcept {
        if (inode_not_available(inode)) 
            return true;

        if (greader_then(offset, inode->size)) 
            return true;

        if (greader_then((offset + length), inode->size)) 
            return true;

        return false;
    }

    [[nodiscard]]
    inline constexpr bool write_fill_at_guard(const kernel::filesystem::Inode* inode,
                                              const uint32_t offset,
                                              const uint32_t length, 
                                              const uint32_t buffer_size) noexcept {
        if (inode_not_available(inode)) 
            return true;

        if (greader_then(offset, inode->size)) 
            return true;

        if (greader_then((offset + length), inode->size)) 
            return true;

        if (less_then(buffer_size, length)) 
            return true;
            
        return false;
    }

    [[nodiscard]]
    inline constexpr bool set_file_content_guard(const kernel::filesystem::Inode* inode,
                                                 const uint32_t length) noexcept {
        if (inode_not_available(inode)) 
            return true;

        if (greader_then(length, kernel::filesystem::MAX_FILE_SIZE)) 
            return true;

        return false;
    }
}

namespace kernel::filesystem
{
    class MoleculeOS_File_System final {
    private:
        static inline runtime::Array<Inode, MAX_FILES_PER_DIRECTORY> inodes;

        static void recalculate_file_size(Inode* inode) noexcept;

        [[nodiscard]] 
        static Inode* allocate_inode() noexcept;

    public:
        [[nodiscard]]
        static bool is_valid_file_name_or_formant_char(const char symbol) noexcept;

        [[nodiscard]]
        static Inode* get_inode_by_name_and_format(const char* filename, 
                                                   const char* format) noexcept;

        [[nodiscard]]
        static Inode* create_file(const char* filename, const char* format) noexcept;

        static void delete_file(Inode* inode) noexcept;

        static bool get_file_content_binary(const Inode* inode, 
                                            uint8_t* out_buffer,
                                            const uint32_t buffer_size) noexcept;

        static bool get_file_content_as_string(const Inode* inode, 
                                               char* out_buffer,
                                               const uint32_t buffer_size) noexcept;

        static bool read_file_binary_at(const Inode* inode,  
                                        uint8_t* out_buffer,
                                        const uint32_t buffer_size, 
                                        const uint32_t offset, 
                                        const uint32_t length) noexcept;

        static bool read_file_as_string_at(const Inode* inode, 
                                          char* out_buffer,
                                          const uint32_t buffer_size, 
                                          const uint32_t offset, 
                                          const uint32_t length) noexcept;

        static bool set_file_content_binary(Inode* inode, 
                                            const uint8_t* in_buffer,
                                            const uint32_t buffer_size) noexcept; 

        static bool set_file_content_as_string(Inode* inode, 
                                              const char* in_buffer,
                                              const uint32_t buffer_size) noexcept;

        static bool write_file_binary_at(Inode* inode,
                                         const uint8_t* in_buffer,
                                         const uint32_t buffer_size,
                                         const uint32_t offset,
                                         const uint32_t length) noexcept;

        static bool write_file_string_at(Inode* inode,
                                         const char* in_buffer,
                                         const uint32_t buffer_size,
                                         const uint32_t offset,
                                         const uint32_t length) noexcept;

        [[nodiscard]]
        static inline constexpr const runtime::Array<Inode, MAX_FILES_PER_DIRECTORY>& get_inodes()
        noexcept {
            return inodes;
        }

        MoleculeOS_File_System() noexcept = default;
        ~MoleculeOS_File_System() noexcept = default;
    };
} // namespace kernel::filesystem
