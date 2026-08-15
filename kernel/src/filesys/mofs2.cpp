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

#include "filesys/mofs2.hpp"

namespace kernel::filesys
{
    uint32_t MoleculeOS_File_System_2::to_fnv1a_hash(const char* txt) noexcept {
        const uint32_t prime_offset = 0x01000193;
        uint32_t hash = 0x811C9DC5;
        
        while (*txt) {
            hash ^= static_cast<uint8_t>(*txt++);
            hash *= prime_offset;
        }

        return hash;
    }
    
    bool MoleculeOS_File_System_2::file_already_exists(const File_Header& file_header, 
                                                       const char* name, 
                                                       const char* format,
                                                       const uint32_t name_hash,
                                                       const uint32_t format_hash) 
                                                       noexcept {
        using namespace stdlib;

        if (!name_and_format_guard(name, format)) [[unlikely]]
            return false;

        if ((file_header.name_hash == name_hash) && (file_header.format_hash == format_hash))
            if ((String_Manipulation::compare_strings(name, 
                                                      file_header.file_name.data()) == 
                status::EQUAL_TO) &&
                (String_Manipulation::compare_strings(format, 
                                                      file_header.file_format.data()) == 
                status::EQUAL_TO))
                return true;
        
        return false;
    }

    File_Header* MoleculeOS_File_System_2::create_file(const char* name, 
                                                  const char* format, 
                                                  const uint32_t byte_size) 
                                                  noexcept {
        using namespace stdlib;

        if (!name_and_format_guard(name, format)) [[unlikely]]
            return nullptr;

        const uint32_t name_hash   = to_fnv1a_hash(name);
        const uint32_t format_hash = to_fnv1a_hash(format);

        for (uint32_t i = 0; i < file_header_table.size(); i++) {
            if (file_already_exists(file_header_table[i], 
                                    name, 
                                    format,
                                    name_hash,
                                    format_hash)) [[unlikely]]
                return nullptr;

            if (file_header_table[i].file_data_ptr == nullptr) {
                void* ptr; 
                heap::Block_Allocator::allocate(ptr, byte_size);
                if (ptr == nullptr) [[unlikely]]
                    return nullptr;

                file_header_table[i].used_data_byte_size = 0;
                file_header_table[i].file_byte_size      = byte_size;
                file_header_table[i].file_data_ptr       = ptr;
                file_header_table[i].format_hash         = format_hash;
                file_header_table[i].name_hash           = name_hash;

                String_Manipulation::copy_string(file_header_table[i].file_format.data(), format);               
                String_Manipulation::copy_string(file_header_table[i].file_name.data(), name);

                return &file_header_table[i];
            }
        }

        return nullptr;
    }

    bool MoleculeOS_File_System_2::delete_file(const char* name, 
                                               const char* format) noexcept {
        using namespace stdlib;

        if (!name_and_format_guard(name, format)) [[unlikely]]
            return false;
            
        const uint32_t name_hash   = to_fnv1a_hash(name);
        const uint32_t format_hash = to_fnv1a_hash(format);

        for (uint32_t i = 0; i < file_header_table.size(); i++) {
            if (file_already_exists(file_header_table[i], 
                                    name, 
                                    format,
                                    name_hash,
                                    format_hash)) [[likely]] {
                if (file_header_table[i].file_data_ptr != nullptr) [[unlikely]]
                    heap::Block_Allocator::deallocate(file_header_table[i].file_data_ptr);

                file_header_table[i].file_name.fill('\0');
                file_header_table[i].file_format.fill('\0');
                file_header_table[i].name_hash      = 0;
                file_header_table[i].format_hash    = 0;
                file_header_table[i].file_byte_size = 0;
                file_header_table[i].file_data_ptr  = nullptr;

                return true;
            }
        }

        return false;
    }

    File_Header* MoleculeOS_File_System_2::find_file(const char* name,
                                                const char* format) noexcept {
        if (!name_and_format_guard(name, format)) [[unlikely]]
            return nullptr;

        const uint32_t name_hash   = to_fnv1a_hash(name);
        const uint32_t format_hash = to_fnv1a_hash(format);

        for (uint32_t i = 0; i < file_header_table.size(); i++)
            if (file_already_exists(file_header_table[i],
                                    name,
                                    format,
                                    name_hash,
                                    format_hash)) [[likely]]
                return &file_header_table[i];

        return nullptr;
    }

    bool MoleculeOS_File_System_2::write_file(File_Header* file_header,
                                              const uint32_t offset,
                                              const uint32_t length,
                                              const uint32_t data_size,
                                              const uint8_t* data) noexcept {
        using namespace stdlib;
        
        if (file_header == nullptr) [[unlikely]]
            return false;

        if (file_header->file_data_ptr == nullptr) [[unlikely]]
            return false;

        if (offset >= file_header->file_byte_size) [[unlikely]]
            return false;

        if (offset + length > file_header->file_byte_size) [[unlikely]]
            return false;

        if (data_size < length) [[unlikely]]
            return false;

        uint8_t* dest_ptr = static_cast<uint8_t*>(file_header->file_data_ptr);
        Memory_Manipulation::copy_memory_block(dest_ptr + offset, 
                                               data, 
                                               length);

        return true;
    }

    bool MoleculeOS_File_System_2::append_file(File_Header* file_header,
                                               const uint8_t* data,
                                               const uint32_t data_size) noexcept {
        using namespace stdlib;

        if (file_header == nullptr) [[unlikely]]
            return false;

        if (file_header->file_data_ptr == nullptr) [[unlikely]]
           return false;

        if (file_header->used_data_byte_size + data_size > file_header->file_byte_size) [[unlikely]]
            return false;

        uint8_t* data_ptr = static_cast<uint8_t*>(file_header->file_data_ptr);
        Memory_Manipulation::copy_memory_block(data_ptr + file_header->used_data_byte_size,
                                               data,
                                               data_size);

        file_header->used_data_byte_size += data_size;
        return true;
    }

    bool MoleculeOS_File_System_2::clear_file(File_Header* file_header) noexcept {
        using namespace stdlib;

        if (file_header == nullptr) [[unlikely]]
            return false;

        if (file_header->file_data_ptr == nullptr) [[unlikely]]
            return false;

        uint8_t* data_ptr = static_cast<uint8_t*>(file_header->file_data_ptr);
        if (data_ptr == nullptr) [[unlikely]]
            return false;

        Memory_Manipulation::set_memory_block(data_ptr, 0, file_header->file_byte_size);

        file_header->used_data_byte_size = 0;
        return true;
    }

    bool MoleculeOS_File_System_2::rename_file(const char* old_name,
                                               const char* old_format,
                                               const char* new_name,
                                               const char* new_format) noexcept {
        using namespace stdlib;

        if (!name_and_format_guard(old_name, old_format)) [[unlikely]]
            return false;

        if (!name_and_format_guard(new_name, new_format)) [[unlikely]]
            return false;

        File_Header* file_header = find_file(old_name, old_format);
        if (file_header == nullptr) [[unlikely]]
            return false;

        if (find_file(new_name, new_format) != nullptr) [[unlikely]]
            return false;

        String_Manipulation::copy_string(file_header->file_name.data(), new_name);
        String_Manipulation::copy_string(file_header->file_format.data(), new_format);

        file_header->name_hash   = to_fnv1a_hash(new_name);
        file_header->format_hash = to_fnv1a_hash(new_format);

        return true;
    }

    bool MoleculeOS_File_System_2::copy_file(const char* src_name,
                                             const char* src_format,
                                             const char* dest_name,
                                             const char* dest_format) noexcept {
        using namespace stdlib;

        if (!name_and_format_guard(src_name, src_format)) [[unlikely]]
            return false;

        if (!name_and_format_guard(dest_name, dest_format)) [[unlikely]]
            return false;

        File_Header* src_file_header = find_file(src_name, src_format);
        if (src_file_header == nullptr) [[unlikely]]
            return false;

        if (find_file(dest_name, dest_format) != nullptr) [[unlikely]]
            return false;

        File_Header* dest_file_header = create_file(dest_name, 
                                         dest_format, 
                                         src_file_header->file_byte_size);
        if (dest_file_header == nullptr) [[unlikely]]
            return false;

        uint8_t* src_ptr  = static_cast<uint8_t*>(src_file_header->file_data_ptr);
        uint8_t* dest_ptr = static_cast<uint8_t*>(dest_file_header->file_data_ptr);
        if (src_ptr == nullptr || dest_ptr == nullptr) [[unlikely]]
            return false;

        Memory_Manipulation::copy_memory_block(dest_ptr,
                                               src_ptr,
                                               src_file_header->file_byte_size);

        dest_file_header->used_data_byte_size = src_file_header->used_data_byte_size;
        return true;
    }

    bool MoleculeOS_File_System_2::read_file(File_Header* file_header,
                                             uint8_t* buffer,
                                             const uint32_t buffer_size,
                                             const uint32_t offset,
                                             const uint32_t length) noexcept {
        using namespace stdlib;

        if (file_header == nullptr) [[unlikely]]
            return false;

        if (file_header->file_data_ptr == nullptr) [[unlikely]]
           return false;

        if (buffer == nullptr) [[unlikely]]
            return false;

        if (offset >= file_header->file_byte_size) [[unlikely]]
            return false;

        if (offset + length > file_header->file_byte_size) [[unlikely]]
            return false;

        if (buffer_size < length) [[unlikely]]
            return false;

        uint8_t* src_ptr = static_cast<uint8_t*>(file_header->file_data_ptr);
        if (src_ptr == nullptr) [[unlikely]]
           return false;

        Memory_Manipulation::copy_memory_block(buffer, 
                                               src_ptr + offset, 
                                               length);

        return true;
    }

    bool MoleculeOS_File_System_2::resize_file_size(File_Header* file_header,
                                                    const uint32_t new_size) 
                                                    noexcept {
        if (file_header == nullptr) [[unlikely]]
            return false;

        if (file_header->file_data_ptr == nullptr) [[unlikely]]
            return false;

        if (heap::Block_Allocator::reallocate(file_header->file_data_ptr, new_size)
            != status::SUCCESS) [[unlikely]] {
            return false;
        }

        file_header->file_byte_size = new_size;

        if (file_header->used_data_byte_size > new_size)
            file_header->used_data_byte_size = new_size;

        return true;
    }

    bool MoleculeOS_File_System_2::is_valid_name_or_format_char(const char symbol)
                                                                noexcept {
        if (((symbol >= 'A') && (symbol <= 'Z')) ||
            ((symbol >= 'a') && (symbol <= 'z')) ||
            ((symbol >= '0') && (symbol <= '9')) ||
             (symbol == '_') || (symbol == '-')) [[likely]]
            return true;

        return false;
    };
} // namespace kernel::filesys
