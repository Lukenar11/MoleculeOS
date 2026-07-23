/*
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

    One method are placed in the header 
    because they are so small that the compiler can inline them.

    Some functions are placed in the header 
    because they are so small that the compiler can inline them.
*/

#pragma once

#include "filesys/helpers.hpp"
#include <array.hpp>
#include <kernel_api.hpp>
#include <memory_manip.hpp>
#include <string_manip.hpp>
#include <stdint.h>

namespace
{
    [[nodiscard]]
    inline constexpr bool File_not_available(const kernel::filesys::File* file)
                                              noexcept {
        return !file || !file->in_use; 
    }

    [[nodiscard]]
    inline constexpr bool less_then(const uint32_t a, 
                                    const uint32_t b) noexcept {
        return a < b; 
    }

    [[nodiscard]]
    inline constexpr bool greater_then(const uint32_t a, 
                                       const uint32_t b) noexcept {
        return a > b; 
    }

    [[nodiscard]]
    inline constexpr bool read_file_at_guard(const kernel::filesys::File* file,
                                             const uint32_t offset, 
                                             const uint32_t length) noexcept {
        if (File_not_available(file)) 
            return true;

        if (greater_then(offset, file->size)) 
            return true;

        if (greater_then((offset + length), file->size)) 
            return true;

        return false;
    }

    [[nodiscard]]
    inline constexpr bool write_fill_at_guard(const kernel::filesys::File* file,
                                              const uint32_t offset,
                                              const uint32_t length, 
                                              const uint32_t buffer_size) noexcept {
        if (File_not_available(file)) 
            return true;

        if (greater_then(offset, file->size)) 
            return true;

        if (greater_then((offset + length), file->size)) 
            return true;

        if (less_then(buffer_size, length)) 
            return true;
            
        return false;
    }

    [[nodiscard]]
    inline constexpr bool set_file_content_guard(const kernel::filesys::File* file,
                                                 const uint32_t length) noexcept {
        if (File_not_available(file)) 
            return true;

        if (greater_then(length, kernel::filesys::MAX_FILE_SIZE)) 
            return true;

        return false;
    }
}

namespace kernel::filesys
{
    class MoleculeOS_File_sys final {
    private:
        static inline runtime::Array<File, MAX_FILES_PER_DIRECTORY> files;

        static void recalculate_file_size(File* file) noexcept;

        [[nodiscard]] 
        static File* allocate_File() noexcept;

    public:
        [[nodiscard]]
        static bool is_valid_file_name_or_formant_char(const char symbol) noexcept;

        [[nodiscard]]
        static File* get_file_by_name_and_format(const char* filename, 
                                                 const char* format) noexcept;

        [[nodiscard]]
        static File* create_file(const char* filename, const char* format) noexcept;

        static void delete_file(File* file) noexcept;

        static bool get_file_content_binary(const File* file, 
                                            uint8_t* out_buffer,
                                            const uint32_t buffer_size) noexcept;

        static bool get_file_content_as_string(const File* file, 
                                               char* out_buffer,
                                               const uint32_t buffer_size) noexcept;

        static bool read_file_binary_at(const File* file,  
                                        uint8_t* out_buffer,
                                        const uint32_t buffer_size, 
                                        const uint32_t offset, 
                                        const uint32_t length) noexcept;

        static bool read_file_as_string_at(const File* file, 
                                           char* out_buffer,
                                           const uint32_t buffer_size, 
                                           const uint32_t offset, 
                                           const uint32_t length) noexcept;

        static bool set_file_content_binary(File* file, 
                                            const uint8_t* in_buffer,
                                            const uint32_t buffer_size) noexcept; 

        static bool set_file_content_as_string(File* file, 
                                               const char* in_buffer,
                                               const uint32_t buffer_size) noexcept;

        static bool write_file_binary_at(File* file,
                                         const uint8_t* in_buffer,
                                         const uint32_t buffer_size,
                                         const uint32_t offset,
                                         const uint32_t length) noexcept;

        static bool write_file_string_at(File* file,
                                         const char* in_buffer,
                                         const uint32_t buffer_size,
                                         const uint32_t offset,
                                         const uint32_t length) noexcept;

        [[nodiscard]]
        static inline constexpr const runtime::Array<File, MAX_FILES_PER_DIRECTORY>& get_files()
        noexcept {
            return files;
        }

        MoleculeOS_File_sys() noexcept  = default;
        ~MoleculeOS_File_sys() noexcept = default;
    };
} // namespace kernel::filesys
