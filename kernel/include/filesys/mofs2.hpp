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
#include "allocator/block.hpp"
#include <array.hpp>
#include <types.hpp>
#include <string_manip.hpp>

namespace
{
    inline bool name_and_format_guard(const char* name, 
                                      const char* format) noexcept {
        using namespace stdlib;
        using namespace kernel::filesys;

        if (!name || !format) [[unlikely]] 
            return false;

        uint32_t name_length;
        uint32_t format_length;
        String_Manipulation::get_string_length(name_length, name);
        String_Manipulation::get_string_length(format_length, format);

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
        static inline stdlib::Array<File_Header, FILE_HEADER_TABLE_ENTRYS> file_header_table;

        static uint32_t to_fnv1a_hash(const char* txt) noexcept;

        static bool file_already_exists(const File_Header& file_header, 
                                        const char* name, 
                                        const char* format,
                                        const uint32_t name_hash,
                                        const uint32_t format_hash) 
                                        noexcept;

    public:
        static inline constexpr const stdlib::Array<File_Header, FILE_HEADER_TABLE_ENTRYS>& get_file_header_table() 
        noexcept {
            return file_header_table;
        };

        static inline constexpr File_Header& get_file_header_entry(const uint32_t index) noexcept {
            return file_header_table[index];
        }

        static inline constexpr File_Header& set_file_header_entry(const File_Header& file_header, 
                                                        const uint32_t index) noexcept {
            return file_header_table[index] = file_header;
        }

        static File_Header* create_file(const char* name, 
                                   const char* format, 
                                   const uint32_t byte_size) noexcept;

        static bool delete_file(const char* file_name, 
                                const char* file_format) noexcept;

        static File_Header* find_file(const char* name,
                                 const char* format) noexcept;

        static bool write_file(File_Header* file_header,
                               const uint32_t offset,
                               const uint32_t length,
                               const uint32_t data_size,
                               const uint8_t* data) noexcept;

        static bool append_file(File_Header* file_header,
                                const uint8_t* data,
                                const uint32_t data_size) noexcept;

        static bool clear_file(File_Header* file_header) noexcept;

        static bool rename_file(const char* old_name,
                                const char* old_format,
                                const char* new_name,
                                const char* new_format) noexcept;

        static bool copy_file(const char* src_name,
                              const char* src_format,
                              const char* dest_name,
                              const char* dest_format) noexcept;

        static bool read_file(File_Header* file_header,
                              uint8_t* buffer,
                              const uint32_t buffer_size,
                              const uint32_t offset,
                              const uint32_t length) noexcept;

        static bool resize_file_size(File_Header* file_header,
                                     const uint32_t new_size) noexcept;

        static bool is_valid_name_or_format_char(const char symbol) noexcept;
    };
} // namespace kernel::filesys
