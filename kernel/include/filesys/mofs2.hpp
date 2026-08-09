/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file contains the internal implementation of the file system.
    The file system is a flat custom file system residing directly in RAM.

NOTES:
    Since the file system resides directly in RAM, 
    all files—along with their contents—are deleted upon 
    shutting down or restarting the sys.

    One function are placed in the header 
    because they are so small that the compiler can inline them.

    Some methods are placed in the header 
    because they are so small that the compiler can inline them.
*/

#pragma once

#include "filesys/helpers.hpp"
#include "heap/block.hpp"
#include <array.hpp>
#include <types.h>
#include <string_manip.hpp>

namespace
{
    inline bool name_and_format_guard(const char* name, 
                                      const char* format) noexcept {
        using namespace runtime;
        using namespace kernel::filesys;

        if (!name || !format) [[unlikely]] 
            return false;

        const uint32_t name_length   = String_Manipulation::get_string_length(name);
        const uint32_t format_length = String_Manipulation::get_string_length(format);

        if (name[0] == '\0') [[unlikely]]
            return false;

        if (name_length == 0) [[unlikely]]
            return false;

        if (name_length > MAX_FILE_NAME_LENGTH) [[unlikely]] 
            return false;

        if (format[0] == '\0') [[unlikely]]
            return false;

        if (format_length == 0) [[unlikely]]
            return false;

        if (format_length > MAX_FILE_FORMAT_LENGTH) [[unlikely]]
            return false;

        return true;
    }
}

namespace kernel::filesys
{
    class MoleculeOS_File_System_2 final {
    private:
        static inline runtime::Array<I_Node, INODE_TABLE_ENTRYS> inode_table;

        static uint32_t to_fnv1a_hash(const char* txt) noexcept;

        static bool file_already_exists(const I_Node& inode, 
                                        const char* name, 
                                        const char* format,
                                        const uint32_t name_hash,
                                        const uint32_t format_hash) 
                                        noexcept;

    public:
        static inline constexpr const runtime::Array<I_Node, INODE_TABLE_ENTRYS>& get_inode_table() 
        noexcept {
            return inode_table;
        };

        static inline constexpr I_Node& get_inode_entry(const uint32_t index) noexcept {
            return inode_table[index];
        }

        static inline constexpr I_Node& set_inode_entry(const I_Node& inode, 
                                                        const uint32_t index) noexcept {
            return inode_table[index] = inode;
        }

        static I_Node* create_file(const char* name, 
                                   const char* format, 
                                   const uint32_t byte_size) noexcept;

        static bool delete_file(const char* file_name, 
                                const char* file_format) noexcept;

        static I_Node* find_file(const char* name,
                                 const char* format) noexcept;

        static bool write_file(I_Node* inode,
                               const uint32_t offset,
                               const uint32_t length,
                               const uint32_t data_size,
                               const uint8_t* data) noexcept;

        static bool append_file(I_Node* inode,
                                const uint8_t* data,
                                const uint32_t data_size) noexcept;

        static bool clear_file(I_Node* inode) noexcept;

        static bool rename_file(const char* old_name,
                                const char* old_format,
                                const char* new_name,
                                const char* new_format) noexcept;

        static bool copy_file(const char* src_name,
                              const char* src_format,
                              const char* dest_name,
                              const char* dest_format) noexcept;

        static bool read_file(I_Node* inode,
                              uint8_t* buffer,
                              const uint32_t buffer_size,
                              const uint32_t offset,
                              const uint32_t length) noexcept;

        static bool resize_file_size(I_Node* inode,
                                     const uint32_t new_size) noexcept;

        static bool is_valid_name_or_format_char(const char symbol) noexcept;
    };
} // namespace kernel::filesys
