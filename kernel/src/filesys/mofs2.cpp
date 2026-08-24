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
    uint32_t MoleculeOS_File_System_2::to_fnv1a_hash(_IN_ const char* txt) noexcept {
        const uint32_t prime_offset = 0x01000193;
        uint32_t hash               = 0x811C9DC5;
        
        while (*txt) [[likely]] {
            hash ^= static_cast<uint8_t>(*txt++);
            hash *= prime_offset;
        }

        return hash;
    }

    status_t MoleculeOS_File_System_2::validate_name_and_format(_IN_ const char* name, 
                                                                _IN_ const char* format) 
                                                                noexcept {
        using namespace stdlib;
        using namespace kernel::filesys;

        status_t status;
        uint32_t name_length;
        uint32_t format_length;

        if (!name || !format) [[unlikely]] {
            status = status::NULL_POINTER;
            goto cleanup;
        }

        String_Manipulation::get_string_length(name_length, name);
        String_Manipulation::get_string_length(format_length, format);

        if (name[0] == '\0' || format[0] == '\0') [[unlikely]] {
            status = status::INVALID_PARAMETER;
            goto cleanup;
        }

        if (name_length == 0 ||
            format_length == 0) [[unlikely]] {
            status = status::INVALID_PARAMETER;
            goto cleanup;
        }

        if (name_length > MAX_FILE_NAME_LENGTH ||
            format_length > MAX_FILE_FORMAT_LENGTH) [[unlikely]] {
            status = status::FS_OUT_OF_SPACE;
            goto cleanup;
        }

        status = status::SUCCESS;

    cleanup:
        return status;
    }
    
    status_t MoleculeOS_File_System_2::file_already_exists(_IN_ const File_Header& file_header, 
                                                           _IN_ const char* name, 
                                                           _IN_ const char* format,
                                                           _IN_ const uint32_t name_hash,
                                                           _IN_ const uint32_t format_hash) 
                                                           noexcept {
        using namespace stdlib;

        status_t status;
        status_t name_status;
        status_t format_status;

        status = validate_name_and_format(name, format);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        if (file_header.name_hash == name_hash && 
            file_header.format_hash == format_hash) {
            name_status = String_Manipulation::compare_strings(name, 
                                                               file_header.file_name.data());
            format_status = String_Manipulation::compare_strings(format, 
                                                                 file_header.file_format.data());
            if (name_status == status::EQUAL_TO && 
                format_status == status::EQUAL_TO) {
                status = status::ALREADY_EXISTS;
                goto cleanup;
            }
        }

        status = status::NOT_FOUND;
        
    cleanup:
        return status;
    }

    status_t MoleculeOS_File_System_2::check_file_not_exists(_IN_ const char* name,
                                                             _IN_ const char* format,
                                                             _IN_ uint32_t name_hash,
                                                             _IN_ uint32_t format_hash) 
                                                             noexcept {
        status_t status;

        for (uint32_t i = 0; i < file_header_table.size(); i++) [[likely]] {
            if (file_already_exists(file_header_table[i],
                                    name,
                                    format,
                                    name_hash,
                                    format_hash) == status::ALREADY_EXISTS) {
                status = status::ALREADY_EXISTS;
                goto cleanup;
            }
        }

        status = status::SUCCESS;

    cleanup:
        return status;
    }

    status_t MoleculeOS_File_System_2::find_free_file_header(_OUT_ uint32_t& index) 
                                                             noexcept {
        status_t status;

        for (uint32_t i = 0; i < file_header_table.size(); i++) [[likely]] {
            if (!file_header_table[i].file_data_ptr) {
                index  = i;
                status = status::SUCCESS;
                goto cleanup;
            }
        }

        status = status::FS_OUT_OF_SPACE;
    
    cleanup:
        return status;
    }

    status_t MoleculeOS_File_System_2::init_file_header(_OUT_ File_Header*& header,
                                                        _IN_  uint32_t index,
                                                        _IN_  const char* name,
                                                        _IN_  const char* format,
                                                        _IN_  uint32_t name_hash,
                                                        _IN_  uint32_t format_hash,
                                                        _IN_  uint32_t byte_size) 
                                                        noexcept {
        using namespace stdlib;

        status_t status;
        void* ptr;
        File_Header& file_header = file_header_table[index];

        status = heap::Block_Allocator::allocate(ptr, byte_size);
        if (status != status::SUCCESS || !ptr) [[unlikely]] {
            header = nullptr;
            goto cleanup;
        }

        file_header.used_data_byte_size = 0;
        file_header.file_byte_size      = byte_size;
        file_header.file_data_ptr       = ptr;
        file_header.name_hash           = name_hash;
        file_header.format_hash         = format_hash;

        String_Manipulation::copy_string(file_header.file_name.data(), name);
        String_Manipulation::copy_string(file_header.file_format.data(), format);

        header = &file_header;

        status = status::SUCCESS;

    cleanup:
        return status;
    }

    status_t MoleculeOS_File_System_2::find_file_for_deletion(_IN_ const char* name,
                                                              _IN_ const char* format,
                                                              _OUT_ uint32_t& index) noexcept {
        status_t status;
        uint32_t name_hash;
        uint32_t format_hash;

        status = validate_name_and_format(name, format);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        name_hash   = to_fnv1a_hash(name);
        format_hash = to_fnv1a_hash(format);

        for (uint32_t i = 0; i < file_header_table.size(); i++) [[likely]] {
            if (file_already_exists(file_header_table[i],
                                    name,
                                    format,
                                    name_hash,
                                    format_hash) == status::ALREADY_EXISTS) {
                index  = i;
                status = status::SUCCESS;
                goto cleanup;
            }
        }

        status = status::NOT_FOUND;

    cleanup:
        return status;
    }

    status_t MoleculeOS_File_System_2::clear_file_data(_IN_ const uint32_t i) 
                                                       noexcept {
        using namespace heap;

        status_t status;

        if (!file_header_table[i].file_data_ptr) [[unlikely]] {
            status = status::EMPTY;
            goto cleanup;
        }

        status = Block_Allocator::deallocate(file_header_table[i].file_data_ptr);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        file_header_table[i].file_name.fill('\0');
        file_header_table[i].file_format.fill('\0');
        file_header_table[i].name_hash      = 0;
        file_header_table[i].format_hash    = 0;
        file_header_table[i].file_byte_size = 0;
        file_header_table[i].file_data_ptr  = nullptr;

        status = status::SUCCESS;

    cleanup:
        return status;
    }

    status_t MoleculeOS_File_System_2::create_file(_OUT_ File_Header*& file_header,
                                                   _IN_  const char* name,
                                                   _IN_  const char* format,
                                                   _IN_  uint32_t byte_size) 
                                                   noexcept {
        status_t status;
        uint32_t index;
        uint32_t name_hash;
        uint32_t format_hash;

        if (byte_size == 0) [[unlikely]] {
            status = status::INVALID_PARAMETER;
            goto cleanup;
        }

        name_hash   = to_fnv1a_hash(name);
        format_hash = to_fnv1a_hash(format);

        status = check_file_not_exists(name, format, name_hash, format_hash);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        status = find_free_file_header(index);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        status = init_file_header(file_header,
                                  index,
                                  name,
                                  format,
                                  name_hash,
                                  format_hash,
                                  byte_size);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        status = status::SUCCESS;

    cleanup:
        return status;
    }

    status_t MoleculeOS_File_System_2::delete_file(_IN_ const char* name, 
                                                   _IN_ const char* format) 
                                                   noexcept {
        using namespace stdlib;

        status_t status;
        uint32_t index;

        status = validate_name_and_format(name, format);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }
            
        status = find_file_for_deletion(name, format, index);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        status = clear_file_data(index);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        status = status::SUCCESS;

    cleanup:
        return status;
    }

    status_t MoleculeOS_File_System_2::find_file(_OUT_ File_Header*& file_header,
                                                 _IN_  const char* name,
                                                 _IN_  const char* format) 
                                                 noexcept {
        status_t status;
        uint32_t name_hash;  
        uint32_t format_hash;

        status = validate_name_and_format(name, format);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        name_hash   = to_fnv1a_hash(name);
        format_hash = to_fnv1a_hash(format);

        for (uint32_t i = 0; i < file_header_table.size(); i++) [[likely]] {
            if (file_already_exists(file_header_table[i],
                                    name,
                                    format,
                                    name_hash,
                                    format_hash) == status::ALREADY_EXISTS) [[likely]] {
                file_header = &file_header_table[i];
                status      = status::SUCCESS;
                goto done;
            }
        }

    cleanup:
        file_header = nullptr;
        status      = status::NOT_FOUND;

    done:
        return status;
    }

    status_t MoleculeOS_File_System_2::write_file(_IN_ File_Header* file_header,
                                                  _IN_ const uint32_t offset,
                                                  _IN_ const uint32_t length,
                                                  _IN_ const uint32_t data_size,
                                                  _IN_ const uint8_t* data) 
                                                  noexcept {
        using namespace stdlib;

        status_t status;
        uint8_t* dest_ptr;
        
        if (!file_header) [[unlikely]] {
            status = status::NOT_FOUND;
            goto cleanup;
        }

        if (!file_header->file_data_ptr) [[unlikely]] {
            status = status::EMPTY;
            goto cleanup;
        }

        if (offset >= file_header->file_byte_size ||
            offset + length > file_header->file_byte_size) [[unlikely]] {
            status = status::FS_OUT_OF_SPACE;
            goto cleanup;
        }

        if (data_size < length) [[unlikely]] {
            status = status::INVALID_PARAMETER;
            goto cleanup;
        }

        dest_ptr = static_cast<uint8_t*>(file_header->file_data_ptr);
        Memory_Manipulation::copy_memory_block(dest_ptr + offset, 
                                               data, 
                                               length);

        status = status::SUCCESS;

    cleanup:
        return status;
    }

    status_t MoleculeOS_File_System_2::append_file(_IN_ File_Header* file_header,
                                                   _IN_ const uint8_t* data,
                                                   _IN_ const uint32_t data_size) 
                                                   noexcept {
        using namespace stdlib;

        status_t status;
        uint8_t* data_ptr;
        uint8_t* dest_ptr;

        if (!file_header) [[unlikely]] {
            status = status::NOT_FOUND;
            goto cleanup;
        }

        if (!file_header->file_data_ptr) [[unlikely]] {
            status = status::EMPTY;
            goto cleanup;
        }

        if (file_header->used_data_byte_size + data_size > 
            file_header->file_byte_size) [[unlikely]] {
            status = status::FS_OUT_OF_SPACE;
            goto cleanup;
        }

        data_ptr = static_cast<uint8_t*>(file_header->file_data_ptr);
        dest_ptr = data_ptr + file_header->used_data_byte_size;
        Memory_Manipulation::copy_memory_block(dest_ptr, data, data_size);

        file_header->used_data_byte_size += data_size;

        status = status::SUCCESS;

    cleanup:
        return status;
    }

    status_t MoleculeOS_File_System_2::clear_file(_IN_ File_Header* file_header) 
                                                  noexcept {
        using namespace stdlib;

        status_t status;
        uint8_t* data_ptr;

        if (!file_header) [[unlikely]] {
            status = status::NOT_FOUND;
            goto cleanup;
        }

        if (!file_header->file_data_ptr) [[unlikely]] {
            status = status::EMPTY;
            goto cleanup;
        }

        data_ptr = static_cast<uint8_t*>(file_header->file_data_ptr);
        if (!data_ptr) [[unlikely]] {
            return false;
        }

        Memory_Manipulation::set_memory_block(data_ptr, 
                                              0, 
                                              file_header->file_byte_size);
        file_header->used_data_byte_size = 0;

        status = status::SUCCESS;

    cleanup:
        return status;
    }

    status_t MoleculeOS_File_System_2::rename_file(_IN_ const char* old_name,
                                                   _IN_ const char* old_format,
                                                   _IN_ const char* new_name,
                                                   _IN_ const char* new_format)
                                                   noexcept {
        using namespace stdlib;

        status_t status;
        File_Header* file_header;

        status = validate_name_and_format(old_name, old_format);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        status = validate_name_and_format(new_name, new_format);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        status = find_file(file_header, old_name, old_format);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        status = find_file(file_header, new_name, new_format);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        String_Manipulation::copy_string(file_header->file_name.data(), new_name);
        String_Manipulation::copy_string(file_header->file_format.data(), new_format);

        file_header->name_hash   = to_fnv1a_hash(new_name);
        file_header->format_hash = to_fnv1a_hash(new_format);

        status = status::SUCCESS;

    cleanup:
        return status;
    }

    status_t MoleculeOS_File_System_2::copy_file(_IN_ const char* src_name,
                                                 _IN_ const char* src_format,
                                                 _IN_ const char* dest_name,
                                                 _IN_ const char* dest_format) 
                                                 noexcept {
        using namespace stdlib;

        status_t status;
        File_Header* src_file_header;
        File_Header* dest_file_header;
        uint8_t* src_ptr; 
        uint8_t* dest_ptr;

        status = validate_name_and_format(src_name, src_format);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        status = validate_name_and_format(dest_name, dest_format);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        status = find_file(src_file_header, src_name, src_format);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        status = find_file(dest_file_header, dest_name, dest_format);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        create_file(dest_file_header, 
                    dest_name, 
                    dest_format, 
                    src_file_header->file_byte_size);
        if (!dest_file_header) [[unlikely]] {
            status = status::FAIL;
            goto cleanup;
        }

        src_ptr  = static_cast<uint8_t*>(src_file_header->file_data_ptr);
        dest_ptr = static_cast<uint8_t*>(dest_file_header->file_data_ptr);
        if (!src_ptr || !dest_ptr) [[unlikely]] {
            status = status::EMPTY;
            goto cleanup;
        }

        Memory_Manipulation::copy_memory_block(dest_ptr,
                                               src_ptr,
                                               src_file_header->file_byte_size);

        dest_file_header->used_data_byte_size = src_file_header->used_data_byte_size;

        status = status::SUCCESS;

    cleanup:
        return status;
    }

    status_t MoleculeOS_File_System_2::read_file(_IN_ File_Header* file_header,
                                                 _IN_ uint8_t* buffer,
                                                 _IN_ const uint32_t buffer_size,
                                                 _IN_ const uint32_t offset,
                                                 _IN_ const uint32_t length) 
                                                 noexcept {
        using namespace stdlib;

        status_t status;
        uint8_t* src_ptr;

        if (!file_header || !buffer) [[unlikely]] {
            status = status::NULL_POINTER;
            goto cleanup;
        }

        if (!file_header->file_data_ptr) [[unlikely]] {
            status = status::EMPTY;
            goto cleanup;
        }

        if (offset >= file_header->file_byte_size ||
            offset + length > file_header->file_byte_size) [[unlikely]] {
            status = status::FS_OUT_OF_SPACE;
            goto cleanup;
        }

        if (buffer_size < length) [[unlikely]]{
            status = status::INVALID_PARAMETER;
            goto cleanup;
        }

        src_ptr = static_cast<uint8_t*>(file_header->file_data_ptr);
        if (!src_ptr) [[unlikely]] {
            status = status::FAIL;
            goto cleanup;
        }

        Memory_Manipulation::copy_memory_block(buffer, 
                                               src_ptr + offset, 
                                               length);
        status = status::SUCCESS;

    cleanup:
        return status;
    }

    status_t MoleculeOS_File_System_2::resize_file_size(_IN_ File_Header* file_header,
                                                        _IN_ const uint32_t new_size) 
                                                        noexcept {
        status_t status;

        if (!file_header) [[unlikely]] {
            status = status::NULL_POINTER;
            goto cleanup;
        }

        if (file_header->file_data_ptr) [[unlikely]] {
            status = status::EMPTY;
            goto cleanup;
        }

        if (new_size == 0) [[unlikely]] {
            status = status::INVALID_PARAMETER;
            goto cleanup;
        }

        status = heap::Block_Allocator::reallocate(file_header->file_data_ptr, 
                                                   new_size);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        file_header->file_byte_size = new_size;
        if (file_header->used_data_byte_size > new_size) [[likely]] {
            file_header->used_data_byte_size = new_size;
        }

        status = status::SUCCESS;

    cleanup:
        return status;
    }

    bool MoleculeOS_File_System_2::is_valid_name_or_format_char(_IN_ const char symbol)
                                                                noexcept {
        if (((symbol >= 'A') && (symbol <= 'Z')) ||
            ((symbol >= 'a') && (symbol <= 'z')) ||
            ((symbol >= '0') && (symbol <= '9')) ||
             (symbol == '_') || (symbol == '-')) [[likely]]
            return true;

        return false;
    };
} // namespace kernel::filesys
