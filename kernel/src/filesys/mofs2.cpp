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

    Some methods are placed in the header 
    because they are so small that the compiler can inline them.
*/


#include "filesys/mofs2.hpp"


namespace kernel::filesys
{
    /**
     * @brief Hashes a string to FNV-1a.
     *
     * @note This is used to create file name and format hashes for
     *       fast file searching and comparing.
     *
     * @param text String to hash.
     *
     * @return FNV-1a hash of `text`.
     */
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


    /**
     * @brief Validates file names and file formats for most class methods.
     *
     * @param name   File name to validate.
     * @param format File format to validate.
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *          If `name` is a `nullptr`.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_A`
     *          If `name` is empty.
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_B`
     *          If `format` is a `nullptr`.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
     *          If `format` is empty.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_A`
     *          If the length of `name` is `0`.
     * 
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
     *          If the length of `format` is `0`.
     *
     * @retval `status::FS_OUT_OF_SPACE | status::flags::PARAM_A`
     *          If the `name` is to long.
     *
     * @retval `status::FS_OUT_OF_SPACE | status::flags::PARAM_B`
     *          If `format` is to long.
     *
     * @retval `status::SUCCESS`
     *          Default case.
     */
    [[nodiscard]]
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

        if (name_length == 0) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_A;
            goto cleanup;
        }

        if (format_length == 0) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_B;
            goto cleanup;
        }

        if (name_length > filesys::MAX_FILE_NAME_LENGTH) [[unlikely]] {
            status = status::FS_OUT_OF_SPACE | status::flags::PARAM_A;
            goto cleanup;
        }

        if (format_length > filesys::MAX_FILE_FORMAT_LENGTH) [[unlikely]] {
            status = status::FS_OUT_OF_SPACE | status::flags::PARAM_B;
            goto cleanup;
        }

        status = status::SUCCESS;

    cleanup:
        return status;
    }
    

    /**
     * @brief Checks whether a file entry already exists.
     *
     * @param file_entry    File entry to compare.
     * @param name          File name to search for.
     * @param format        File format to search for.
     * @param name_hash     File name to search for as hash.
     * @param format_hash   File format to search for as hash.
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *          If `name` is a `nullptr`.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_A`
     *          If `name` is empty.
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_B`
     *          If `format` is a `nullptr`.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
     *          If `format` is empty.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_A`
     *          If the length of `name` is `0`.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
     *          If the length of `format` is `0`.
     *
     * @retval `status::FS_OUT_OF_SPACE | status::flags::PARAM_A`
     *          If `name` is to long.
     *
     * @retval `status::FS_OUT_OF_SPACE | status::flags::PARAM_B`
     *          If `format` is to long.
     *
     * @retval `status::ALREADY_EXISTS`
     *          If the file already exists.
     *
     * @retval `status::NOT_FOUND`
     *          Default case.
     */
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
            const char* file_name_ptr   = file_entry.file_name.data();
            const char* file_format_ptr = file_entry.file_format.data();

            name_status = String_Manipulation::compare_strings(name, 
                                                               file_name_ptr);

            format_status = String_Manipulation::compare_strings(format, 
                                                                 file_format_ptr);

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


    /**
     * @brief Validates that a file entry does not already exist.
     *
     * @param name        File name to search for.
     * @param format      File format to search for.
     * @param name_hash   File name to search for as hash.
     * @param format_hash File format to search for as hash.
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *          If `name` is a `nullptr`.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_A`
     *          If `name` is empty.
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_B`
     *          If `format` is a `nullptr`.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
     *          If `format` is empty.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_A`
     *          If the length of `name` is `0`.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
     *          If the length of `format` is `0`.
     *
     * @retval `status::FS_OUT_OF_SPACE | status::flags::PARAM_A`
     *          If the `name` is to long.
     *
     * @retval `status::FS_OUT_OF_SPACE | status::flags::PARAM_B`
     *          If the `format` is to long.
     *
     * @retval `status::ALREADY_EXISTS`
     *          If a matching file entry already exists.
     *
     * @retval `status::SUCCESS`
     *          Default case.
     */
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


    /**
     * @brief Finds the first free file entry slot.
     *
     * @param index Output index of the free file entry slot.
     *
     * @retval `status::FS_OUT_OF_SPACE`
     *          If no free file entry slot exists.
     *
     * @retval `status::SUCCESS`
     *          Default case.
     */
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


    /**
     * @brief Initializes a file entry.
     *
     * @param file_entry  Output pointer to the initialized file entry.
     * @param name        File name for the file entry.
     * @param format      File format for the file entry.
     * @param index       File entry table index.
     * @param name_hash   File name as `hash`.
     * @param format_hash File format as `hash`.
     * @param byte_size   Byte size of the file.
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *          If `name` is a `nullptr`.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_A`
     *          If `name` is empty.
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_B`
     *          If `format` is a `nullptr`.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
     *          If `format` is empty.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_A`
     *          If the length of `name` is `0`.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
     *          If the length of `format` is `0`.
     *
     * @retval `status::FS_OUT_OF_SPACE | status::flags::PARAM_A`
     *          If `name` is to long.
     *
     * @retval `status::FS_OUT_OF_SPACE | status::flags::PARAM_B`
     *          If `format` is to long.
     *
     * @retval `status::HEAP_EXHAUSTED`
     *          If the memory pool does not have enough free space or
     *          `byte_size` is to large.
     *
     * @retval `status::SUCCESS`
     *          Default case.
     */
    status_t
    MoleculeOS_File_System_2::init_file_entry(_OUT_ File_Entry*& file_entry,
                                              _IN_  const char* name,
                                              _IN_  const char* format,
                                              _IN_  uint32_t name_hash,
                                              _IN_  uint32_t format_hash,
                                              _IN_  uint32_t index,
                                              _IN_  uint32_t byte_size) 
                                              noexcept {
        using namespace stdlib;

        status_t status;
        void* file_data_ptr;
        File_Entry& entry = file_entry_table[index];

        status = validate_name_and_format(name, format);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        status = heap::Block_Allocator::allocate(file_data_ptr, byte_size);
        if (status != status::SUCCESS || !file_data_ptr) [[unlikely]] {
            file_entry = nullptr;
            goto cleanup;
        }

        entry.used_data_byte_size = 0;
        entry.file_byte_size      = byte_size;
        entry.file_data_ptr       = file_data_ptr;
        entry.name_hash           = name_hash;
        entry.format_hash         = format_hash;

        status = String_Manipulation::copy_string(entry.file_name.data(), 
                                                  name);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        status = String_Manipulation::copy_string(entry.file_format.data(), 
                                                  format);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        file_entry = &entry;
        status     = status::SUCCESS;

    cleanup:
        return status;
    }


    /**
     * @brief Finds the first file entry that matches the given name and 
     *        format.
     *
     * @param index   Output index of the matching file entry.
     * @param name    File name to search for.
     * @param format  File format to search for.
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *          If `name` is a `nullptr`.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_A`
     *          If `name` is empty.
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_B`
     *          If `format` is a `nullptr`.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
     *          If `format` is empty.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_A`
     *          If the length of `name` is `0`.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
     *          If the length of `format` is `0`.
     *
     * @retval `status::FS_OUT_OF_SPACE | status::flags::PARAM_A`
     *          If `name` is to long.
     *
     * @retval `status::FS_OUT_OF_SPACE | status::flags::PARAM_B`
     *          If `format` is to long.
     *
     * @retval `status::NOT_FOUND`
     *          If no matching file entry exists.
     *
     * @retval `status::SUCCESS`
     *          Default case.
     */
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


    /**
     * @brief Clears the file data of a file entry.
     *
     * @param file_index File entry table index.
     *
     * @retval `status::INVALID_PARAMETER`
     *          If `file_index` is out of range.
     *
     * @retval `status::EMPTY`
     *          If the file is empty.
     * 
     * @retval `status::NULL_POINTER` 
     *          If the file can't be deallocated.
     * 
     * @retval `status::HEAP_CORRUPTED` 
     *          If a deallocation error has occurred.
     *
     * @retval `status::SUCCESS`
     *          Default case.
     */
    status_t 
    MoleculeOS_File_System_2::clear_file_data(_IN_ const uint32_t file_index) 
                                              noexcept {
        using namespace heap;

        status_t status;
        void* file_data_ptr;

        if (file_index >= FILE_TABLE_ENTRYS) [[unlikely]] {
            status = status::INVALID_PARAMETER;
            goto cleanup;
        }

        file_data_ptr = file_entry_table[file_index].file_data_ptr;
        if (!file_data_ptr) [[unlikely]] {
            status = status::EMPTY;
            goto cleanup;
        }

        status = Block_Allocator::deallocate(file_data_ptr);
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

        
    /** 
     * @brief Creates a file with a specific name, format and size.
     * 
     * @param file_entry Output pointer to the file entry of the created file.
     * @param name       Name of the file to create.
     * @param format     Format of the file to create.
     * @param byte_size  Byte size of the file to create.
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *          If `name` is a `nullptr`.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_A`
     *          If `name` is empty.
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_B`
     *          If `format` is a `nullptr`.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
     *          If `format` is empty.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_A`
     *          If the length of `name` is `0`.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
     *          If the length of `format` is `0`.
     *
     * @retval `status::FS_OUT_OF_SPACE | status::flags::PARAM_A`
     *          If `name` is to long.
     *
     * @retval `status::FS_OUT_OF_SPACE | status::flags::PARAM_B`
     *          If `format` is to long.
     *
     * @retval `status::HEAP_EXHAUSTED`
     *          If the memory pool does not have enough free space or
     *          `byte_size` is to large.
     * 
     * @retval `status::FS_OUT_OF_SPACE`
     *          If no free file entry slot exists.
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     */
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
                                 name_hash,
                                 format_hash,
                                 index,
                                 byte_size);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        status = status::SUCCESS;

    cleanup:
        return status;
    }


    /** 
     * @brief Deletes a file with a specific name and format.
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *          If `name` is a `nullptr`.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_A`
     *          If `name` is empty.
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_B`
     *          If `format` is a `nullptr`.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
     *          If `format` is empty.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_A`
     *          If the length of `name` is `0`.
     * 
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
     *          If the length of `format` is `0`.
     *
     * @retval `status::FS_OUT_OF_SPACE | status::flags::PARAM_A`
     *          If the `name` is to long.
     *
     * @retval `status::FS_OUT_OF_SPACE | status::flags::PARAM_B`
     *          If `format` is to long.
     * 
     * @retval `status::INVALID_PARAMETER`
     *          If `index` of the file is out of range.
     *
     * @retval `status::EMPTY`
     *          If the file is empty.
     * 
     * @retval `status::NULL_POINTER` 
     *          If the file can't be deallocated.
     * 
     * @retval `status::HEAP_CORRUPTED` 
     *          If a deallocation error has occurred.
     *
     * @retval `status::SUCCESS`
     *          Default case.
     */
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


    /** 
     * @brief Finds a file with a specific name and format.
     * 
     * @param file_entry Output pointer to file entry of the found file.
     * @param name       Name of the file to find.
     * @param format     Format of the file to find.
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *          If `name` is a `nullptr`.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_A`
     *          If `name` is empty.
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_B`
     *          If `format` is a `nullptr`.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
     *          If `format` is empty.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_A`
     *          If the length of `name` is `0`.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
     *          If the length of `format` is `0`.
     *
     * @retval `status::FS_OUT_OF_SPACE | status::flags::PARAM_A`
     *          If `name` is to long.
     *
     * @retval `status::FS_OUT_OF_SPACE | status::flags::PARAM_B`
     *          If `format` is to long.
     *
     * @retval `status::ALREADY_EXISTS`
     *          If the file already exists.
     *
     * @retval `status::NOT_FOUND`
     *          If the file was not found.
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     */
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


    /**
     * @brief Writes data byte wise to a file.
     * 
     * @param file_entry     Pointer to the file entry to write.
     * @param data           Pointer of the data to write.
     * @param offset         Range begin from the file data area to write.
     * @param length         Range length from the file data area to write.
     * @param data_byte_size Byte size if the data to write.
     * 
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_A`
     *          If `file_entry` is a `nullptr`.
     * 
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
     *          If `data_ptr` is a `nullptr`.
     * 
     * @retval `status::EMPTY`
     *          iF The file to write has no allocated data area.
     * 
     * @retval `status::FS_OUT_OF_SPACE`
     *          If the range of `offset` or `length` are not valid.
     * 
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_D`
     *          If the `data_byte_size` is less then `length`.
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_
    status_t 
    MoleculeOS_File_System_2::write_file(_IN_ File_Entry* file_entry,
                                         _IN_ const uint8_t* data_ptr,
                                         _IN_ const uint32_t offset,
                                         _IN_ const uint32_t length,
                                         _IN_ const uint32_t data_byte_size) 
                                         noexcept {
        using namespace stdlib;

        status_t status;
        uint8_t* destination_ptr;
        
        if (!file_entry) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_A;
            goto cleanup;
        }

        if (!data_ptr) [[unlikely]] {
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

        if (data_byte_size < length) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_D;
            goto cleanup;
        }

        destination_ptr = static_cast<uint8_t*>(file_entry->file_data_ptr) + 
                          offset;

        status = Memory_Manipulation::copy_memory_block(destination_ptr, 
                                                        data_ptr, 
                                                        length);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        status = status::SUCCESS;

    cleanup:
        return status;
    }


    /** 
     * @brief Appends the data to the content of a file.
     * 
     * @param file_entry     Pointer to the file entry of the file to append.
     * @param data_ptr       Pointer to the data to append.
     * @param data_byte_size Byte size to the data to append.
     *  
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_A`
     *          If `file_entry` is a `nullptr`.
     * 
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
     *          If `data_ptr` is a `nullptr`.
     * 
     * @retval `status::EMPTY`
     *          iF The file to write has no allocated data area.
     * 
     * @retval `status::FS_OUT_OF_SPACE`
     *          If the file is not gib enough for the data append.
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_
    status_t 
    MoleculeOS_File_System_2::append_file(_IN_ File_Entry* file_entry,
                                          _IN_ const uint8_t* data_ptr,
                                          _IN_ const uint32_t data_byte_size) 
                                          noexcept {
        using namespace stdlib;

        status_t status;
        uint8_t* file_data_ptr;
        uint8_t* destination_ptr;

        if (!file_entry) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_A;
            goto cleanup;
        }

        if (!data_ptr) [[unlikely]] {
            status = status::NULL_POINTER | status::flags::PARAM_B;
            goto cleanup;
        }

        if (!file_entry->file_data_ptr) [[unlikely]] {
            status = status::EMPTY;
            goto cleanup;
        }

        if (file_entry->used_data_byte_size + data_byte_size > 
            file_entry->file_byte_size) [[unlikely]] {
            status = status::FS_OUT_OF_SPACE;
            goto cleanup;
        }

        file_data_ptr   = static_cast<uint8_t*>(file_entry->file_data_ptr);
        destination_ptr = file_data_ptr + file_entry->used_data_byte_size;

        status = Memory_Manipulation::copy_memory_block(destination_ptr, 
                                                        data_ptr, 
                                                        data_byte_size);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        file_entry->used_data_byte_size += data_byte_size;

        status = status::SUCCESS;

    cleanup:
        return status;
    }


    /**
     * @brief Clears the content of a file.
     *
     * @param file_entry Pointer to the file entry of the file to clear.
     *
     * @retval `status::NOT_FOUND`
     *          If `file_entry` is a `nullptr`.
     *
     * @retval `status::EMPTY`
     *          If the file is empty.
     *
     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_
    status_t 
    MoleculeOS_File_System_2::clear_file(_IN_ File_Entry* file_entry) 
                                         noexcept {
        using namespace stdlib;

        status_t status;
        uint8_t* file_data_ptr;
        uint32_t file_byte_size;

        if (!file_entry) [[unlikely]] {
            status = status::NOT_FOUND;
            goto cleanup;
        }

        file_data_ptr = static_cast<uint8_t*>(file_entry->file_data_ptr);

        if (!file_entry->file_data_ptr ||
            !file_data_ptr) [[unlikely]] {
            status = status::EMPTY;
            goto cleanup;
        }

        file_byte_size = file_entry->file_byte_size;

        status = Memory_Manipulation::set_memory_block(file_data_ptr, 
                                                       0x00, 
                                                       file_byte_size);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        file_entry->used_data_byte_size = 0;

        status = status::SUCCESS;

    cleanup:
        return status;
    }


    /** 
     * @brief Renames a already existing file.
     * 
     * @param new_name   New name of the file to rename.
     * @param new_format New format of the file to rename.
     * @param old_name   Name of the file to rename.
     * @param old_format Format of the file to rename.
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *          If `old_name` is a `nullptr`.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_A`
     *          If `old_name` is empty.
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_B`
     *          If `old_format` is a `nullptr`.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
     *          If `old_format` is empty.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_A`
     *          If the length of `old_name` is `0`.
     * 
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
     *          If the length of `old_format` is `0`.
     *
     * @retval `status::FS_OUT_OF_SPACE | status::flags::PARAM_A`
     *          If the `old_name` is to long.
     *
     * @retval `status::FS_OUT_OF_SPACE | status::flags::PARAM_B`
     *          If `old_format` is to long.
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *          If `new_name` is a `nullptr`.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_A`
     *          If `new_name` is empty.
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_B`
     *          If `new_format` is a `nullptr`.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
     *          If `new_format` is empty.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_A`
     *          If the length of `new_name` is `0`.
     * 
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
     *          If the length of `new_format` is `0`.
     *
     * @retval `status::FS_OUT_OF_SPACE | status::flags::PARAM_A`
     *          If the `new_name` is to long.
     *
     * @retval `status::FS_OUT_OF_SPACE | status::flags::PARAM_B`
     *          If `new_format` is to long.
     * 
     * @retval `status::ALREADY_EXISTS`
     *          If a file with the new name already exists.
     * 
     * @retval `status::NOT_FOUND`
     *          If the the file to rename is not found.
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_
    status_t
    MoleculeOS_File_System_2::rename_file(_IN_ const char* new_name,
                                          _IN_ const char* new_format,
                                          _IN_ const char* old_name,
                                          _IN_ const char* old_format) 
                                          noexcept {
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


    /**
     * @brief copys the content fo a file to another file.
     * 
     * @param destination_name   Name of the destination file.
     * @param destination_format Format of the destination file.
     * @param source_name        Name of the source file.
     * @param source_format      Format of the source file.
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *          If `destination_name` is a `nullptr`.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_A`
     *          If `destination_name` is empty.
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_B`
     *          If `destination_format` is a `nullptr`.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
     *          If `destination_format` is empty.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_A`
     *          If the length of `destination_name` is `0`.
     * 
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
     *          If the length of `destination_format` is `0`.
     *
     * @retval `status::FS_OUT_OF_SPACE | status::flags::PARAM_A`
     *          If the `destination_name` is to long.
     *
     * @retval `status::FS_OUT_OF_SPACE | status::flags::PARAM_B`
     *          If `destination_format` is to long.
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *          If `source_name` is a `nullptr`.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_A`
     *          If `source_name` is empty.
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_B`
     *          If `source_format` is a `nullptr`.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
     *          If `source_format` is empty.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_A`
     *          If the length of `source_name` is `0`.
     * 
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
     *          If the length of `source_format` is `0`.
     *
     * @retval `status::FS_OUT_OF_SPACE | status::flags::PARAM_A`
     *          If the `source_name` is to long.
     *
     * @retval `status::FS_OUT_OF_SPACE | status::flags::PARAM_B`
     *          If `source_format` is to long.
     * 
     * @retval `status::NOT_FOUND`
     *          If the source file are not found.
     * 
     * @retval `status::EMPTY`
     *          If the source file are empty.
     * 
     * @retval `status::EMPTY`
     *          If the destination file has not allocated memory.
     * 
     * @retval `status::HEAP_EXHAUSTED` 
     *          If the memory pool does not have enough free space or 
     *          byte_size is to large.
     * 
     * @retval `status::FS_OUT_OF_SPACE` 
     *          If no free file entry slot exists.
     * 
     * @retval `status::SUCCESS`
     *          Default case. 
     */
    _API_
    status_t
    MoleculeOS_File_System_2::copy_file(_IN_ const char* destination_name,
                                        _IN_ const char* destination_format,
                                        _IN_ const char* source_name,
                                        _IN_ const char* source_format) 
                                        noexcept {
        using namespace stdlib;

        status_t status;
        File_Entry* source_file_entry;
        File_Entry* destination_file_entry;
        uint8_t* source_ptr;
        uint8_t* destination_ptr;
        uint32_t source_byte_size;

        status = validate_name_and_format(source_name, source_format);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        status = validate_name_and_format(destination_name, 
                                          destination_format);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        status = find_file(source_file_entry, source_name, source_format);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        source_byte_size = source_file_entry->file_byte_size;

        status = find_file(destination_file_entry, 
                           destination_name, 
                           destination_format);
        if (status == status::SUCCESS) [[likely]] {
            if (source_byte_size > destination_file_entry->file_byte_size) {
                status = resize_file_size(destination_file_entry, 
                                          source_byte_size);
                if (status != status::SUCCESS) [[unlikely]] {
                    goto cleanup;
                }
            }
        }
        else if (status == status::NOT_FOUND) [[unlikely]] {
            status = create_file(destination_file_entry,
                                 destination_name,
                                 destination_format,
                                 source_byte_size);
            if (status != status::SUCCESS) [[unlikely]] {
                goto cleanup;
            }
        }
        else {
            goto cleanup;
        }

        source_ptr      = static_cast<uint8_t*>(source_file_entry->file_data_ptr);
        destination_ptr = static_cast<uint8_t*>(destination_file_entry->file_data_ptr);

        if (!source_ptr || !destination_ptr) [[unlikely]] {
            status = status::EMPTY;
            goto cleanup;
        }

        status = Memory_Manipulation::copy_memory_block(destination_ptr,
                                                        source_ptr,
                                                        source_byte_size);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        destination_file_entry->
        used_data_byte_size = source_file_entry->used_data_byte_size;

        status = status::SUCCESS;

    cleanup:
        return status;
    }


    /**
     * @brief Reads the content of a file.
     *
     * @param file_entry   Pointer to the file entry to read.
     * @param buffer_ptr   Pointer to the output buffer.
     * @param buffer_size  Size of the output buffer.
     * @param offset       Read begin position.
     * @param length       Count of bytes to read.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_A`
     *          If `file_entry` is a `nullptr`.
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_B`
     *          If `buffer_ptr` is a `nullptr`.
     *
     * @retval `status::EMPTY`
     *          If the file is empty.
     *
     * @retval `status::FS_OUT_OF_SPACE`
     *          If the read range is outside the used file content.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_C`
     *          If `buffer_size` is less than `length`.
     *
     * @retval `status::FAIL`
     *          If the read operation fails.
     *
     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_
    status_t 
    MoleculeOS_File_System_2::read_file(_IN_ File_Entry* file_entry,
                                        _IN_ uint8_t* buffer_ptr,
                                        _IN_ const uint32_t buffer_size,
                                        _IN_ const uint32_t offset,
                                        _IN_ const uint32_t length) 
                                        noexcept {
        using namespace stdlib;

        status_t status;
        uint8_t* source_ptr;

        if (!file_entry) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_A;
            goto cleanup;
        }

        if (!buffer_ptr) [[unlikely]] {
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
            status = status::INVALID_PARAMETER | status::flags::PARAM_C;
            goto cleanup;
        }

        source_ptr = static_cast<uint8_t*>(file_entry->file_data_ptr);
        if (!source_ptr) [[unlikely]] {
            status = status::FAIL;
            goto cleanup;
        }

        status = Memory_Manipulation::copy_memory_block(buffer_ptr, 
                                                        source_ptr + offset, 
                                                        length);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        status = status::SUCCESS;

    cleanup:
        return status;
    }


    /**
     * @brief Changes the size of a file.
     *
     * @param file_entry    Pointer to the file entry to change.
     * @param new_byte_size New byte size of the file.
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_A`
     *          If `file_entry` is a `nullptr`.
     *
     * @retval `status::EMPTY`
     *          If the file is empty.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
     *          If `new_byte_size` is `0`.
     *
     * @retval `status::HEAP_EXHAUSTED`
     *          If the memory pool does not have enough free space or
     *          `new_byte_size` is to large.
     *
     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_
    status_t 
    MoleculeOS_File_System_2::resize_file_size(_INOUT_ File_Entry* file_entry,
                                               _IN_    const uint32_t new_byte_size) 
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

        if (new_byte_size == 0) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_B;
            goto cleanup;
        }

        status = heap::Block_Allocator::reallocate(file_entry->file_data_ptr, 
                                                   new_byte_size);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        file_entry->file_byte_size = new_byte_size;
        if (file_entry->used_data_byte_size > new_byte_size) [[likely]] {
            file_entry->used_data_byte_size = new_byte_size;
        }

        status = status::SUCCESS;

    cleanup:
        return status;
    }

    
    /**
     * @brief Validates if a character is valid for file names and formats.
     *
     * @param symbol Character to validate.
     *
     * @return `true` if `symbol` is valid for file names or formats,
     *         otherwise `false`.
     */
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
