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


#pragma once

#include <array.hpp>
#include <types.hpp>
#include <sal.hpp>
#include <status.hpp>


namespace kernel::filesys
{
    constexpr uint32_t MAX_FILE_NAME_LENGTH   = 8;
    constexpr uint32_t MAX_FILE_FORMAT_LENGTH = 4;
    constexpr uint32_t FILE_TABLE_ENTRYS      = 256;

    struct File_Entry final {
        stdlib::Array<char, MAX_FILE_NAME_LENGTH + 1> file_name;
        stdlib::Array<char, MAX_FILE_FORMAT_LENGTH + 1> file_format;
        uint32_t name_hash           = 0;
        uint32_t format_hash         = 0;
        uint32_t used_data_byte_size = 0;
        uint32_t file_byte_size      = 0;
        void* file_data_ptr          = nullptr;
    };


    /**
     * @brief Sets a file entry with a specific index.
     * 
     * @param file_entry data to set the file entry.
     * @param file_index Index of the file entry to get.
     */
    _API_  
    void 
    set_file_entry(_IN_ const File_Entry& file_entry, 
                   _IN_ const uint32_t file_index) noexcept;


    /**
     * @brief Gets a file entry with a specific index.
     * 
     * @param file_index Index of the file entry to get.
     */
    _API_   
    File_Entry&
    get_file_entry(_IN_ const uint32_t file_index) noexcept;
    
    
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
    create_file(_OUT_ File_Entry*& file_entry,
                _IN_  const char* const name, 
                _IN_  const char* const format, 
                _IN_  const uint32_t byte_size) noexcept;


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
    delete_file(_IN_ const char* const name, 
                _IN_ const char* const format) noexcept;

              
    /** 
     * @brief Finds a file with a specific name and format.
     * 
     * @param file_entry Output pointer to file entry of the found file.
     * @param name       Name of the file to find.
     * @param format     Format of the file to find.
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_B`
     *          If `name` is a `nullptr`.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
     *          If `name` is empty.
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_C`
     *          If `format` is a `nullptr`.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_C`
     *          If `format` is empty.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
     *          If the length of `name` is `0`.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_C`
     *          If the length of `format` is `0`.
     *
     * @retval `status::FS_OUT_OF_SPACE | status::flags::PARAM_B`
     *          If `name` is to long.
     *
     * @retval `status::FS_OUT_OF_SPACE | status::flags::PARAM_C`
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
    find_file(_OUT_ File_Entry*& file_entry,
              _IN_  const char* const name,
              _IN_  const char* const format) noexcept;


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
    write_file(_IN_ const File_Entry* const file_entry,
               _IN_ const uint8_t* const data,
               _IN_ const uint32_t offset,
               _IN_ const uint32_t length,
               _IN_ const uint32_t data_size) noexcept;


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
    append_file(_IN_ File_Entry* file_entry,
                _IN_ const uint8_t* const data,
                _IN_ const uint32_t data_size) noexcept;

            
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
    clear_file(_IN_ File_Entry* const file_entry) noexcept;


    /** 
     * @brief Renames a already existing file.
     * 
     * @param new_name   New name of the file to rename.
     * @param new_format New format of the file to rename.
     * @param old_name   Name of the file to rename.
     * @param old_format Format of the file to rename.
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_C`
     *          If `old_name` is a `nullptr`.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_C`
     *          If `old_name` is empty.
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_D`
     *          If `old_format` is a `nullptr`.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_D`
     *          If `old_format` is empty.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_C`
     *          If the length of `old_name` is `0`.
     * 
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_D`
     *          If the length of `old_format` is `0`.
     *
     * @retval `status::FS_OUT_OF_SPACE | status::flags::PARAM_C`
     *          If the `old_name` is to long.
     *
     * @retval `status::FS_OUT_OF_SPACE | status::flags::PARAM_D`
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
    rename_file(_IN_ const char* const new_name,
                _IN_ const char* const new_format,
                _IN_ const char* const old_name,
                _IN_ const char* const old_format) noexcept;


    /**
     * @brief Copys the content fo a file to another file.
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
     * @retval `status::NULL_POINTER | status::flags::PARAM_C`
     *          If `source_name` is a `nullptr`.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_C`
     *          If `source_name` is empty.
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_D`
     *          If `source_format` is a `nullptr`.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_D`
     *          If `source_format` is empty.
     *
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_C`
     *          If the length of `source_name` is `0`.
     * 
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_D`
     *          If the length of `source_format` is `0`.
     *
     * @retval `status::FS_OUT_OF_SPACE | status::flags::PARAM_C`
     *          If the `source_name` is to long.
     *
     * @retval `status::FS_OUT_OF_SPACE | status::flags::PARAM_D`
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
    copy_file(_IN_ const char* const destination_name,
              _IN_ const char* const destination_format,
              _IN_ const char* const source_name,
              _IN_ const char* const source_format) noexcept;


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
    read_file(_IN_ const File_Entry* const file_entry,
              _IN_ uint8_t* const buffer_ptr,
              _IN_ const uint32_t buffer_size,
              _IN_ const uint32_t offset,
              _IN_ const uint32_t length) noexcept;


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
    resize_file_size(_INOUT_ File_Entry* const file_entry, 
                     _IN_    const uint32_t new_byte_size) noexcept;


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
    is_valid_name_or_format_char(_IN_ const char symbol) noexcept;
} // namespace kernel::filesys
