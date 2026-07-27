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

    One method are placed in the header 
    because they are so small that the compiler can inline them.

    Some functions are placed in the header 
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
    
    bool MoleculeOS_File_System_2::file_already_exists(const I_Node& inode, 
                                                       const char* name, 
                                                       const char* format,
                                                       const uint32_t name_hash,
                                                       const uint32_t format_hash) 
                                                       noexcept {
        using namespace runtime;

        if ((inode.name_hash == name_hash) && (inode.format_hash == format_hash))
            if ((String_Manipulation::compare_strings(name, inode.file_name.data()) == 0) &&
                (String_Manipulation::compare_strings(format, inode.file_format.data()) == 0))
                return true;
        
        return false;
    }

    I_Node* MoleculeOS_File_System_2::create_file(const char* name, 
                                                  const char* format, 
                                                  const uint32_t byte_size) 
                                                  noexcept {
        using namespace runtime;

        if (!name_and_format_guard(name, format)) [[unlikely]]
            return nullptr;

        const uint32_t name_hash   = to_fnv1a_hash(name);
        const uint32_t format_hash = to_fnv1a_hash(format);

        for (uint32_t i = 0; i < inode_table.size(); i++) {
            if (file_already_exists(inode_table[i], 
                                    name, 
                                    format,
                                    name_hash,
                                    format_hash)) [[unlikely]]
                return nullptr;

            if (inode_table[i].file_data_ptr == nullptr) {
                void* ptr = heap::Block_Allocator::allocate(byte_size);
                if (ptr == nullptr) [[unlikely]]
                    return nullptr;

                inode_table[i].file_byte_size = byte_size;
                inode_table[i].file_data_ptr  = ptr;
                inode_table[i].format_hash    = format_hash;
                inode_table[i].name_hash      = name_hash;

                String_Manipulation::copy_string(inode_table[i].file_format.data(), format);               
                String_Manipulation::copy_string(inode_table[i].file_name.data(), name);

                return &inode_table[i];
            }
        }

        return nullptr;
    }

    bool MoleculeOS_File_System_2::delete_file(const char* name, 
                                               const char* format) noexcept {
        using namespace runtime;

        if (!name_and_format_guard(name, format)) [[unlikely]]
            return false;
            
        const uint32_t name_hash   = to_fnv1a_hash(name);
        const uint32_t format_hash = to_fnv1a_hash(format);

        for (uint32_t i = 0; i < inode_table.size(); i++) {
            if (file_already_exists(inode_table[i], 
                                    name, 
                                    format,
                                    name_hash,
                                    format_hash)) [[likely]] {
                if (inode_table[i].file_data_ptr != nullptr) [[unlikely]]
                    heap::Block_Allocator::deallocate(inode_table[i].file_data_ptr);

                inode_table[i].file_name.fill('\0');
                inode_table[i].file_format.fill('\0');
                inode_table[i].name_hash      = 0;
                inode_table[i].format_hash    = 0;
                inode_table[i].file_byte_size = 0;
                inode_table[i].file_data_ptr  = nullptr;

                return true;
            }
        }

        return false;
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
