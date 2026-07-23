/*
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

#include "filesys/mofs.hpp"

namespace kernel::filesys
{
    [[nodiscard]]
    File* MoleculeOS_File_sys::allocate_File() noexcept {
        for (auto& file : files)
            if (!file.in_use) {
                runtime::Memory_Manipulation::set_memory_block(&file, 
                                                               0, 
                                                               sizeof(file));
                file.in_use = true;
                return &file;
            }

        sys::panic("MOFS: no free Files available");
        return nullptr;
    }

    void MoleculeOS_File_sys::recalculate_file_size(File* file) noexcept {
        if (File_not_available(file)) [[unlikely]]
            return;

        int32_t i = static_cast<int32_t>(MAX_FILE_SIZE) - 1;
        for (; i >= 0; --i)
            if (file->data[i] != 0) {
                file->size = static_cast<uint32_t>(i + 1);
                return;
            }

        file->size = 0;
    }

    bool MoleculeOS_File_sys::is_valid_file_name_or_formant_char(const char symbol)
                                                                 noexcept {
        if (((symbol >= 'A') && (symbol <= 'Z')) ||
            ((symbol >= 'a') && (symbol <= 'z')) ||
            ((symbol >= '0') && (symbol <= '9')) ||
             (symbol == '_') || (symbol == '-')) [[likely]]
            return true;

        return false;
    }

    File* MoleculeOS_File_sys::get_file_by_name_and_format(const char* filename, 
                                                              const char* format) 
                                                              noexcept {
        for (auto& file : files) {
            if (!file.in_use)
                continue;

            if ((runtime::String_Manipulation::compare_strings(file.name, filename) == 0) &&
                (runtime::String_Manipulation::compare_strings(file.format, format) == 0))
                return const_cast<File*>(&file);
        }

        return nullptr;
    }

    File* MoleculeOS_File_sys::create_file(const char* filename, 
                                              const char* format) noexcept {
        if (!filename || (filename[0] == '\0')) [[unlikely]] 
            return nullptr;

        for (uint32_t i = 0; filename[i] != '\0'; i++)
            if (!is_valid_file_name_or_formant_char(filename[i])) [[unlikely]]
                return nullptr;

        for (uint32_t i = 0; format[i] != '\0'; i++)
            if (!is_valid_file_name_or_formant_char(format[i])) [[unlikely]]
                return nullptr;

        for (auto& file : files) {
            if (!file.in_use)
                continue;

            if ((runtime::String_Manipulation::compare_strings(file.name, filename) == 0) &&
                (runtime::String_Manipulation::compare_strings(file.format, 
                                                               format) == 0)) [[unlikely]]
                return nullptr;
        }

        File* file = allocate_File();
        if (!file) [[unlikely]]
            return nullptr;

        runtime::String_Manipulation::copy_string_part(file->name, 
                                                       filename, 
                                                       MAX_FILENAME_LENGTH);
        
        runtime::String_Manipulation::copy_string_part(file->format, 
                                                       format, 
                                                       MAX_FILE_FORMAT_NAME_LENGTH);

        file->name[MAX_FILENAME_LENGTH - 1]           = '\0';
        file->format[MAX_FILE_FORMAT_NAME_LENGTH - 1] = '\0';
        file->size                                    = 0;

        return file;
    }

    void MoleculeOS_File_sys::delete_file(File* file) noexcept {
        file->in_use = false;
        file->size   = 0;

        runtime::Memory_Manipulation::set_memory_block(file->name, 
                                                       '\0', 
                                                       MAX_FILENAME_LENGTH);

        runtime::Memory_Manipulation::set_memory_block(file->format, 
                                                       '\0', 
                                                       MAX_FILE_FORMAT_NAME_LENGTH);

        runtime::Memory_Manipulation::set_memory_block(file->data, 
                                                       0, 
                                                       MAX_FILE_SIZE);
    }

    bool MoleculeOS_File_sys::get_file_content_binary(const File* file, 
                                                         uint8_t* out_buffer,
                                                         const uint32_t buffer_size)
                                                         noexcept {
        if (File_not_available(file)) [[unlikely]] 
            return false;

        if (less_then(buffer_size, file->size)) [[unlikely]] 
            return false;

        runtime::Memory_Manipulation::copy_memory_block(out_buffer, 
                                                        file->data, 
                                                        file->size);
        return true;
    }

    bool MoleculeOS_File_sys::get_file_content_as_string(const File* file, 
                                                            char* out_buffer,
                                                            const uint32_t buffer_size)
                                                            noexcept {
        if (File_not_available(file)) [[unlikely]] 
            return false;

        if (less_then(buffer_size, file->size + 1)) [[unlikely]] 
            return false;

        runtime::Memory_Manipulation::copy_memory_block(out_buffer, 
                                                        file->data, 
                                                        file->size);
        out_buffer[file->size] = '\0';

        return true;
    }

    bool MoleculeOS_File_sys::read_file_binary_at(const File* file,
                                                     uint8_t* out_buffer,
                                                     const uint32_t buffer_size,
                                                     const uint32_t offset,
                                                     const uint32_t length) 
                                                     noexcept {
        if (read_file_at_guard(file, offset, length)) [[unlikely]] 
            return false;

        if (less_then(buffer_size, length)) [[unlikely]] 
            return false;

        runtime::Memory_Manipulation::copy_memory_block(out_buffer, 
                                                        (file->data + offset), 
                                                        length);
    
        return true;
    }

    bool MoleculeOS_File_sys::read_file_as_string_at(const File* file, 
                                                        char* out_buffer,
                                                        const uint32_t buffer_size,
                                                        const uint32_t offset, 
                                                        const uint32_t length) 
                                                        noexcept {
        if (read_file_at_guard(file, offset, length)) [[unlikely]] 
            return false;

        if (less_then(buffer_size, (length + 1))) [[unlikely]] 
            return false;

        runtime::Memory_Manipulation::copy_memory_block(out_buffer, 
                                                        (file->data + offset),
                                                        length);
        out_buffer[length] = '\0';
        return true;
    }

    bool MoleculeOS_File_sys::set_file_content_binary(File* file,
                                                         const uint8_t* in_buffer,
                                                         const uint32_t length)
                                                         noexcept {
        if (set_file_content_guard(file, length)) [[unlikely]]
            return false;

        runtime::Memory_Manipulation::copy_memory_block(file->data, 
                                                        in_buffer, 
                                                        length);
        recalculate_file_size(file);
        return true;
    }

    bool MoleculeOS_File_sys::set_file_content_as_string(File* file,
                                                            const char* in_buffer,
                                                            const uint32_t length) 
                                                            noexcept {
        if (set_file_content_guard(file, length)) [[unlikely]]
            return false;

        runtime::Memory_Manipulation::copy_memory_block(file->data, 
                                                        in_buffer, 
                                                        length);
        recalculate_file_size(file);
        return true;
    }

    bool MoleculeOS_File_sys::write_file_binary_at(File* file,
                                                      const uint8_t* in_buffer,
                                                      const uint32_t buffer_size,
                                                      const uint32_t offset,
                                                      const uint32_t length)
                                                      noexcept {
        if (write_fill_at_guard(file, offset, length, buffer_size)) [[unlikely]]
            return false;

        runtime::Memory_Manipulation::copy_memory_block((file->data + offset), 
                                                        in_buffer, 
                                                        length);
        recalculate_file_size(file);
        return true;
    }

    bool MoleculeOS_File_sys::write_file_string_at(File* file,
                                                      const char* in_buffer,
                                                      const uint32_t buffer_size,
                                                      const uint32_t offset,
                                                      const uint32_t length) 
                                                      noexcept {
        if (write_fill_at_guard(file, offset, length, buffer_size)) [[unlikely]]
            return false;

        runtime::Memory_Manipulation::copy_memory_block((file->data + offset), 
                                                        in_buffer, 
                                                        length);
        recalculate_file_size(file);
        return true;
    }
} // namespace kernel::filesys
