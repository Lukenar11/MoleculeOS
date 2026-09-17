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
    uint32_t 
    MoleculeOS_File_System_2::to_fnv1a_hash(_IN_ const char* text) noexcept {
        const uint32_t prime_offset = 0x01000193;
        uint32_t hash               = 0x811C9DC5;
        
        while (*text) [[likely]] {
            hash ^= static_cast<uint8_t>(*text++);
            hash *= prime_offset;
        }

        return hash;
    }

    status_t 
    MoleculeOS_File_System_2::validate_name_and_format(_IN_ const char* name, 
                                                       _IN_ const char* format) 
                                                       noexcept {
        status_t status;
        uint32_t name_length;
        uint32_t format_length;

        if (!name) [[unlikely]] {
            status = status::NULL_POINTER | status::flags::PARAM_A;
            goto cleanup;
        }

        if (name[0] == '\0') [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_A;
            goto cleanup;
        }

        if (!format) [[unlikely]] {
            status = status::NULL_POINTER | status::flags::PARAM_B;
            goto cleanup;
        }

        if (format[0] == '\0') [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_B;
            goto cleanup;
        }

        status = stdlib::String_Manipulation::get_string_length(name_length, 
                                                                name);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        status = stdlib::String_Manipulation::get_string_length(format_length, 
                                                                format);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        if (name_length == 0 ||
            format_length == 0) [[unlikely]] {
            status = status::INVALID_PARAMETER;
            goto cleanup;
        }

        if (name_length > filesys::MAX_FILE_NAME_LENGTH ||
            format_length > filesys::MAX_FILE_FORMAT_LENGTH) [[unlikely]] {
            status = status::FS_OUT_OF_SPACE;
            goto cleanup;
        }

        status = status::SUCCESS;

    cleanup:
        return status;
    }
    

    status_t 
    MoleculeOS_File_System_2::file_already_exists(_IN_ const File_Entry& file_entry, 
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

        if (file_entry.name_hash == name_hash && 
            file_entry.format_hash == format_hash) {
            name_status = String_Manipulation::compare_strings(name, 
                                                               file_entry.file_name.data());

            format_status = String_Manipulation::compare_strings(format, 
                                                                 file_entry.file_format.data());
            if (name_status == status::EQUAL_TO && 
                format_status == status::EQUAL_TO) [[unlikely]] {
                status = status::ALREADY_EXISTS;
                goto cleanup;
            }
        }

        status = status::NOT_FOUND;
        
    cleanup:
        return status;
    }


    status_t 
    MoleculeOS_File_System_2::check_file_not_exists(_IN_ const char* name,
                                                    _IN_ const char* format,
                                                    _IN_ uint32_t name_hash,
                                                    _IN_ uint32_t format_hash) 
                                                    noexcept {
        status_t status;

        status = validate_name_and_format(name, format);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        for (uint32_t current_entry = 0; 
             current_entry < file_entry_table.size();
             current_entry++) [[likely]] {
            if (file_already_exists(file_entry_table[current_entry],
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


    status_t 
    MoleculeOS_File_System_2::find_free_file_entry(_OUT_ uint32_t& index) 
                                                   noexcept {
        status_t status;

        for (uint32_t current_entry = 0; 
             current_entry < file_entry_table.size(); 
             current_entry++) [[likely]] {
            if (!file_entry_table[current_entry].file_data_ptr) {
                index  = current_entry;
                status = status::SUCCESS;

                goto cleanup;
            }
        }

        status = status::FS_OUT_OF_SPACE;
    
    cleanup:
        return status;
    }


    status_t
    MoleculeOS_File_System_2::init_file_entry(_OUT_ File_Entry*& header,
                                              _IN_  const char* name,
                                              _IN_  const char* format,
                                              _IN_  uint32_t index,
                                              _IN_  uint32_t name_hash,
                                              _IN_  uint32_t format_hash,
                                              _IN_  uint32_t byte_size) 
                                              noexcept {
        using namespace stdlib;

        status_t status;
        void* ptr;
        File_Entry& file_entry = file_entry_table[index];

        status = validate_name_and_format(name, format);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        status = heap::Block_Allocator::allocate(ptr, byte_size);
        if (status != status::SUCCESS || !ptr) [[unlikely]] {
            header = nullptr;
            goto cleanup;
        }

        file_entry.used_data_byte_size = 0;
        file_entry.file_byte_size      = byte_size;
        file_entry.file_data_ptr       = ptr;
        file_entry.name_hash           = name_hash;
        file_entry.format_hash         = format_hash;

        status = String_Manipulation::copy_string(file_entry.file_name.data(), 
                                                  name);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        status = String_Manipulation::copy_string(file_entry.file_format.data(), 
                                                  format);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        header = &file_entry;

        status = status::SUCCESS;

    cleanup:
        return status;
    }


    status_t 
    MoleculeOS_File_System_2::find_file_for_deletion(_OUT_ uint32_t& index,
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

        for (uint32_t current_entry = 0; 
             current_entry < file_entry_table.size(); 
             current_entry++) [[likely]] {
            if (file_already_exists(file_entry_table[current_entry],
                                    name,
                                    format,
                                    name_hash,
                                    format_hash) == status::ALREADY_EXISTS) {
                index  = current_entry;
                status = status::SUCCESS;

                goto cleanup;
            }
        }

        status = status::NOT_FOUND;

    cleanup:
        return status;
    }


    status_t 
    MoleculeOS_File_System_2::clear_file_data(_IN_ const uint32_t file_index) 
                                              noexcept {
        using namespace heap;

        status_t status;

        if (file_index >= FILE_TABLE_ENTRYS) [[unlikely]] {
            status = status::INVALID_PARAMETER;
            goto cleanup;
        }

        if (!file_entry_table[file_index].file_data_ptr) [[unlikely]] {
            status = status::EMPTY;
            goto cleanup;
        }

        status = Block_Allocator::deallocate(file_entry_table[file_index].file_data_ptr);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        file_entry_table[file_index].file_name.fill('\0');
        file_entry_table[file_index].file_format.fill('\0');
        file_entry_table[file_index].name_hash           = 0;
        file_entry_table[file_index].format_hash         = 0;
        file_entry_table[file_index].file_byte_size      = 0;
        file_entry_table[file_index].used_data_byte_size = 0;
        file_entry_table[file_index].file_data_ptr       = nullptr;

        status = status::SUCCESS;

    cleanup:
        return status;
    }

                 
    _API_
    status_t 
    MoleculeOS_File_System_2::create_file(_OUT_ File_Entry*& file_entry,
                                          _IN_  const char* name,
                                          _IN_  const char* format,
                                          _IN_  uint32_t byte_size) noexcept {
        status_t status;
        uint32_t index;
        uint32_t name_hash;
        uint32_t format_hash;

        status = validate_name_and_format(name, format);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        if (byte_size == 0) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_D;
            goto cleanup;
        }

        status = find_free_file_entry(index);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        name_hash   = to_fnv1a_hash(name);
        format_hash = to_fnv1a_hash(format);

        status = check_file_not_exists(name, format, name_hash, format_hash);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        status = init_file_entry(file_entry,
                                 name,
                                 format,
                                 index,
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

                 
    _API_
    status_t 
    MoleculeOS_File_System_2::delete_file(_IN_ const char* name, 
                                          _IN_ const char* format) noexcept {
        using namespace stdlib;

        status_t status;
        uint32_t index;

        status = validate_name_and_format(name, format);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }
            
        status = find_file_for_deletion(index, name, format);
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

                 
    _API_
    status_t 
    MoleculeOS_File_System_2::find_file(_OUT_ File_Entry*& file_entry,
                                        _IN_  const char* name,
                                        _IN_  const char* format) noexcept {
        status_t status;
        uint32_t name_hash;  
        uint32_t format_hash;

        status = validate_name_and_format(name, format);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        name_hash   = to_fnv1a_hash(name);
        format_hash = to_fnv1a_hash(format);

        for (uint32_t current_entry = 0; 
             current_entry < file_entry_table.size(); 
             current_entry++) [[likely]] {
            if (file_already_exists(file_entry_table[current_entry],
                                    name,
                                    format,
                                    name_hash,
                                    format_hash)
                == status::ALREADY_EXISTS) [[likely]] {
                file_entry = &file_entry_table[current_entry];
                status     = status::SUCCESS;
                
                goto done;
            }
        } 

        status = status::NOT_FOUND;

    cleanup:
        file_entry = nullptr;

    done:
        return status;
    }

                 
    _API_
    status_t 
    MoleculeOS_File_System_2::write_file(_IN_ File_Entry* file_entry,
                                         _IN_ const uint8_t* data,
                                         _IN_ const uint32_t offset,
                                         _IN_ const uint32_t length,
                                         _IN_ const uint32_t data_size) 
                                         noexcept {
        using namespace stdlib;

        status_t status;
        uint8_t* dest_ptr;
        
        if (!file_entry) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_A;
            goto cleanup;
        }

        if (!data) [[unlikely]] {
            status = status::NULL_POINTER | status::flags::PARAM_B;
            goto cleanup;
        }

        if (!file_entry->file_data_ptr) [[unlikely]] {
            status = status::EMPTY;
            goto cleanup;
        }

        if (offset >= file_entry->file_byte_size ||
            offset + length > file_entry->file_byte_size ||
            length > file_entry->file_byte_size - offset) [[unlikely]] {
            status = status::FS_OUT_OF_SPACE;
            goto cleanup;
        }

        if (data_size < length) [[unlikely]] {
            status = status::INVALID_PARAMETER;
            goto cleanup;
        }

        dest_ptr = static_cast<uint8_t*>(file_entry->file_data_ptr);
        status   = Memory_Manipulation::copy_memory_block(dest_ptr + offset, 
                                                          data, 
                                                          length);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        status = status::SUCCESS;

    cleanup:
        return status;
    }

                 
    _API_
    status_t 
    MoleculeOS_File_System_2::append_file(_IN_ File_Entry* file_entry,
                                          _IN_ const uint8_t* data,
                                          _IN_ const uint32_t data_size) 
                                          noexcept {
        using namespace stdlib;

        status_t status;
        uint8_t* data_ptr;
        uint8_t* dest_ptr;

        if (!file_entry) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_A;
            goto cleanup;
        }

        if (!data) [[unlikely]] {
            status = status::NULL_POINTER | status::flags::PARAM_B;
            goto cleanup;
        }

        if (!file_entry->file_data_ptr) [[unlikely]] {
            status = status::EMPTY;
            goto cleanup;
        }

        if (file_entry->used_data_byte_size + data_size > 
            file_entry->file_byte_size) [[unlikely]] {
            status = status::FS_OUT_OF_SPACE;
            goto cleanup;
        }

        data_ptr = static_cast<uint8_t*>(file_entry->file_data_ptr);
        dest_ptr = data_ptr + file_entry->used_data_byte_size;

        status = stdlib::Memory_Manipulation::copy_memory_block(dest_ptr, 
                                                                data, 
                                                                data_size);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        file_entry->used_data_byte_size += data_size;

        status = status::SUCCESS;

    cleanup:
        return status;
    }

                 
    _API_
    status_t 
    MoleculeOS_File_System_2::clear_file(_IN_ File_Entry* file_entry) 
                                         noexcept {
        using namespace stdlib;

        status_t status;
        uint8_t* data_ptr;

        if (!file_entry) [[unlikely]] {
            status = status::NOT_FOUND;
            goto cleanup;
        }

        data_ptr = static_cast<uint8_t*>(file_entry->file_data_ptr);

        if (!file_entry->file_data_ptr ||
            !data_ptr) [[unlikely]] {
            status = status::EMPTY;
            goto cleanup;
        }

        status = Memory_Manipulation::set_memory_block(data_ptr, 
                                                       0, 
                                                       file_entry->file_byte_size);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        file_entry->used_data_byte_size = 0;

        status = status::SUCCESS;

    cleanup:
        return status;
    }

       
    _API_
    status_t
    MoleculeOS_File_System_2::rename_file(_IN_ const char* new_name,
                                          _IN_ const char* new_format,
                                          _IN_ const char* old_name,
                                          _IN_ const char* old_format) noexcept {
        using namespace stdlib;

        status_t status;
        File_Entry* file_entry;
        uint32_t new_name_hash;
        uint32_t new_format_hash;

        status = validate_name_and_format(old_name, old_format);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        status = validate_name_and_format(new_name, new_format);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        if (String_Manipulation::compare_strings(old_name, 
                                                 new_name) 
            == status::EQUAL_TO &&
            String_Manipulation::compare_strings(old_format, 
                                                 new_format)
            == status::EQUAL_TO) [[unlikely]] {
            status = status::SUCCESS;
            goto cleanup;
        }

        status = find_file(file_entry, new_name, new_format);
        if (status == status::SUCCESS) [[unlikely]] {
            status = status::ALREADY_EXISTS;
            goto cleanup;
        }  

        status = find_file(file_entry, old_name, old_format);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        new_name_hash   = to_fnv1a_hash(new_name);
        new_format_hash = to_fnv1a_hash(new_format);

        status = check_file_not_exists(new_name, 
                                       new_format, 
                                       new_name_hash, 
                                       new_format_hash);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        status = String_Manipulation::copy_string(file_entry->file_name.data(), 
                                                  new_name);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        status = String_Manipulation::copy_string(file_entry->file_format.data(), 
                                                  new_format);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        file_entry->name_hash   = new_name_hash;
        file_entry->format_hash = new_format_hash;

        status = status::SUCCESS;

    cleanup:
        return status;
    }


    _API_
    status_t
    MoleculeOS_File_System_2::copy_file(_IN_ const char* destination_name,
                                        _IN_ const char* destination_format,
                                        _IN_ const char* source_name,
                                        _IN_ const char* source_format) 
                                        noexcept {
        status_t status;
        File_Entry* src_file_entry;
        File_Entry* dest_file_entry;
        uint8_t* src_ptr;
        uint8_t* dest_ptr;
        uint32_t src_size;

        status = validate_name_and_format(source_name, source_format);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        status = validate_name_and_format(destination_name, 
                                          destination_format);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        status = find_file(src_file_entry, source_name, source_format);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        src_size = src_file_entry->file_byte_size;

        status = find_file(dest_file_entry, 
                           destination_name, 
                           destination_format);
        if (status == status::SUCCESS) [[likely]] {
            if (src_size > dest_file_entry->file_byte_size) {
                status = resize_file_size(dest_file_entry, src_size);
                if (status != status::SUCCESS) [[unlikely]] {
                    goto cleanup;
                }
            }
        }
        else if (status == status::NOT_FOUND) [[unlikely]] {
            status = create_file(dest_file_entry,
                                 destination_name,
                                 destination_format,
                                 src_size);
            if (status != status::SUCCESS) [[unlikely]] {
                goto cleanup;
            }
        }
        else {
            goto cleanup;
        }

        src_ptr  = static_cast<uint8_t*>(src_file_entry->file_data_ptr);
        dest_ptr = static_cast<uint8_t*>(dest_file_entry->file_data_ptr);

        if (!src_ptr || !dest_ptr) [[unlikely]] {
            status = status::EMPTY;
            goto cleanup;
        }

        status = stdlib::Memory_Manipulation::copy_memory_block(dest_ptr,
                                                                src_ptr,
                                                                src_size);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        dest_file_entry->
        used_data_byte_size = src_file_entry->used_data_byte_size;

        status = status::SUCCESS;

    cleanup:
        return status;
    }

                 
    _API_
    status_t 
    MoleculeOS_File_System_2::read_file(_IN_ File_Entry* file_entry,
                                        _IN_ uint8_t* buffer,
                                        _IN_ const uint32_t buffer_size,
                                        _IN_ const uint32_t offset,
                                        _IN_ const uint32_t length) 
                                        noexcept {
        using namespace stdlib;

        status_t status;
        uint8_t* src_ptr;

        if (!file_entry) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_A;
            goto cleanup;
        }

        if (!buffer) [[unlikely]] {
            status = status::NULL_POINTER | status::flags::PARAM_B;
            goto cleanup;
        }

        if (!file_entry->file_data_ptr) [[unlikely]] {
            status = status::EMPTY;
            goto cleanup;
        }

        if (offset >= file_entry->file_byte_size ||
            offset + length > file_entry->file_byte_size ||
            length > file_entry->file_byte_size - offset ||
            offset >= file_entry->used_data_byte_size ||
            length > file_entry->used_data_byte_size - offset) [[unlikely]] {
            status = status::FS_OUT_OF_SPACE;
            goto cleanup;
        }

        if (buffer_size < length) [[unlikely]]{
            status = status::INVALID_PARAMETER;
            goto cleanup;
        }

        src_ptr = static_cast<uint8_t*>(file_entry->file_data_ptr);
        if (!src_ptr) [[unlikely]] {
            status = status::FAIL;
            goto cleanup;
        }

        status = Memory_Manipulation::copy_memory_block(buffer, 
                                                        src_ptr + offset, 
                                                        length);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        status = status::SUCCESS;

    cleanup:
        return status;
    }

                 
    _API_
    status_t 
    MoleculeOS_File_System_2::resize_file_size(_INOUT_ File_Entry* file_entry,
                                               _IN_    const uint32_t new_size) 
                                               noexcept {
        status_t status;

        if (!file_entry) [[unlikely]] {
            status = status::NULL_POINTER | status::flags::PARAM_A;
            goto cleanup;
        }

        if (!file_entry->file_data_ptr) [[unlikely]] {
            status = status::EMPTY;
            goto cleanup;
        }

        if (new_size == 0) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_B;
            goto cleanup;
        }

        status = heap::Block_Allocator::reallocate(file_entry->file_data_ptr, 
                                                   new_size);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        file_entry->file_byte_size = new_size;
        if (file_entry->used_data_byte_size > new_size) [[likely]] {
            file_entry->used_data_byte_size = new_size;
        }

        status = status::SUCCESS;

    cleanup:
        return status;
    }

                 
    _API_
    bool 
    MoleculeOS_File_System_2::is_valid_name_or_format_char(_IN_ const char symbol)
                                                           noexcept {
        bool status;

        if ((symbol >= 'A' && symbol <= 'Z') ||
            (symbol >= 'a' && symbol <= 'z') ||
            (symbol >= '0' && symbol <= '9') ||
             symbol == '_' || symbol == '-') [[likely]] {
            status = true;
        }
        else [[unlikely]] {
            status = false;
        }

        return status;
    };
} // namespace kernel::filesys
