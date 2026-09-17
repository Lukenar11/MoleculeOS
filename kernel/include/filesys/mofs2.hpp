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
#include <types.hpp>
#include <sal.hpp>
#include <status.hpp>
#include <string_manip.hpp>


namespace kernel::filesys
{
    class MoleculeOS_File_System_2 final {
    private:
        static 
        inline 
        stdlib::Array<File_Entry, FILE_TABLE_ENTRYS> file_entry_table;


        static 
        uint32_t 
        to_fnv1a_hash(_IN_ const char* txt) noexcept;


        static
        status_t 
        validate_name_and_format(_IN_ const char* name, 
                                 _IN_ const char* format) noexcept;


        static 
        status_t 
        file_already_exists(_IN_ const File_Entry& file_entry, 
                            _IN_ const char* name, 
                            _IN_ const char* format,
                            _IN_ const uint32_t name_hash,
                            _IN_ const uint32_t format_hash) noexcept;


        static
        status_t
        check_file_not_exists(_IN_ const char* name,
                              _IN_ const char* format,
                              _IN_ uint32_t name_hash,
                              _IN_ uint32_t format_hash) noexcept;


        static
        status_t 
        find_free_file_entry(_OUT_ uint32_t& index) noexcept;


        static
        status_t 
        init_file_entry(_OUT_ File_Entry*& header,
                        _IN_  const char* name,
                        _IN_  const char* format,
                        _IN_  uint32_t index,
                        _IN_  uint32_t name_hash,
                        _IN_  uint32_t format_hash,
                        _IN_  uint32_t byte_size)  noexcept;


        static 
        status_t 
        find_file_for_deletion(_OUT_ uint32_t& index,
                               _IN_  const char* name,
                               _IN_  const char* format) noexcept;


        static
        status_t 
        clear_file_data(_IN_ const uint32_t file_index) noexcept;


    public:              
        _API_
        static 
        inline 
        constexpr 
        File_Entry& 
        get_file_entry(_IN_ const uint32_t file_index) noexcept {
            return file_entry_table[file_index];
        }

                    
        _API_
        static 
        inline 
        constexpr 
        void 
        set_file_entry(_IN_ const File_Entry& file_entry, 
                       _IN_ const uint32_t file_index) noexcept {
            file_entry_table[file_index] = file_entry;
        }

                    
        _API_
        static 
        status_t 
        create_file(_OUT_ File_Entry*& file_entry,
                    _IN_  const char* name, 
                    _IN_  const char* format, 
                    _IN_  const uint32_t byte_size) noexcept;

                    
        _API_
        static 
        status_t 
        delete_file(_IN_ const char* name, 
                    _IN_ const char* format) noexcept;

                    
        _API_
        static 
        status_t 
        find_file(_OUT_ File_Entry*& file_entry,
                  _IN_  const char* name,
                  _IN_  const char* format) noexcept;

                    
        _API_
        static 
        status_t 
        write_file(_IN_ File_Entry* file_entry,
                   _IN_ const uint8_t* data,
                   _IN_ const uint32_t offset,
                   _IN_ const uint32_t length,
                   _IN_ const uint32_t data_size) noexcept;

                    
        _API_
        static 
        status_t 
        append_file(_IN_ File_Entry* file_entry,
                    _IN_ const uint8_t* data,
                    _IN_ const uint32_t data_size) noexcept;

                    
        _API_
        static 
        status_t 
        clear_file(_IN_ File_Entry* file_entry) noexcept;


        static 
        status_t 
        rename_file(_IN_ const char* new_name,
                    _IN_ const char* new_format,
                    _IN_ const char* old_name,
                    _IN_ const char* old_format) noexcept;


        _API_
        static 
        status_t 
        copy_file(_IN_ const char* destination_name,
                  _IN_ const char* destination_format,
                  _IN_ const char* source_name,
                  _IN_ const char* source_format) noexcept;


        _API_
        static 
        status_t 
        read_file(_IN_ File_Entry* file_entry,
                  _IN_ uint8_t* buffer,
                  _IN_ const uint32_t buffer_size,
                  _IN_ const uint32_t offset,
                  _IN_ const uint32_t length) noexcept;


        _API_
        static 
        status_t 
        resize_file_size(_INOUT_ File_Entry* file_entry, 
                         _IN_    const uint32_t new_size) noexcept;


        _API_
        static 
        bool 
        is_valid_name_or_format_char(_IN_ const char symbol) noexcept;
    };
} // namespace kernel::filesys
