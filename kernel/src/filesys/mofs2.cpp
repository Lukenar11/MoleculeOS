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
    
    bool MoleculeOS_File_System_2::file_already_exists(const I_Node& inode, 
                                                       const char* name, 
                                                       const char* format,
                                                       const uint32_t name_hash,
                                                       const uint32_t format_hash) 
                                                       noexcept {
        using namespace stdlib;

        if (!name_and_format_guard(name, format)) [[unlikely]]
            return false;

        if ((inode.name_hash == name_hash) && (inode.format_hash == format_hash))
            if ((String_Manipulation::compare_strings(name, 
                                                      inode.file_name.data()) == 
                status::EQUAL_TO) &&
                (String_Manipulation::compare_strings(format, 
                                                      inode.file_format.data()) == 
                status::EQUAL_TO))
                return true;
        
        return false;
    }

    I_Node* MoleculeOS_File_System_2::create_file(const char* name, 
                                                  const char* format, 
                                                  const uint32_t byte_size) 
                                                  noexcept {
        using namespace stdlib;

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

                inode_table[i].used_data_byte_size = 0;
                inode_table[i].file_byte_size      = byte_size;
                inode_table[i].file_data_ptr       = ptr;
                inode_table[i].format_hash         = format_hash;
                inode_table[i].name_hash           = name_hash;

                String_Manipulation::copy_string(inode_table[i].file_format.data(), format);               
                String_Manipulation::copy_string(inode_table[i].file_name.data(), name);

                return &inode_table[i];
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

    I_Node* MoleculeOS_File_System_2::find_file(const char* name,
                                                const char* format) noexcept {
        if (!name_and_format_guard(name, format)) [[unlikely]]
            return nullptr;

        const uint32_t name_hash   = to_fnv1a_hash(name);
        const uint32_t format_hash = to_fnv1a_hash(format);

        for (uint32_t i = 0; i < inode_table.size(); i++)
            if (file_already_exists(inode_table[i],
                                    name,
                                    format,
                                    name_hash,
                                    format_hash)) [[likely]]
                return &inode_table[i];

        return nullptr;
    }

    bool MoleculeOS_File_System_2::write_file(I_Node* inode,
                                              const uint32_t offset,
                                              const uint32_t length,
                                              const uint32_t data_size,
                                              const uint8_t* data) noexcept {
        using namespace stdlib;
        
        if (inode == nullptr) [[unlikely]]
            return false;

        if (inode->file_data_ptr == nullptr) [[unlikely]]
            return false;

        if (offset >= inode->file_byte_size) [[unlikely]]
            return false;

        if (offset + length > inode->file_byte_size) [[unlikely]]
            return false;

        if (data_size < length) [[unlikely]]
            return false;

        uint8_t* dest_ptr = static_cast<uint8_t*>(inode->file_data_ptr);
        Memory_Manipulation::copy_memory_block(dest_ptr + offset, 
                                               data, 
                                               length);

        return true;
    }

    bool MoleculeOS_File_System_2::append_file(I_Node* inode,
                                               const uint8_t* data,
                                               const uint32_t data_size) noexcept {
        using namespace stdlib;

        if (inode == nullptr) [[unlikely]]
            return false;

        if (inode->file_data_ptr == nullptr) [[unlikely]]
           return false;

        if (inode->used_data_byte_size + data_size > inode->file_byte_size) [[unlikely]]
            return false;

        uint8_t* data_ptr = static_cast<uint8_t*>(inode->file_data_ptr);
        Memory_Manipulation::copy_memory_block(data_ptr + inode->used_data_byte_size,
                                               data,
                                               data_size);

        inode->used_data_byte_size += data_size;
        return true;
    }

    bool MoleculeOS_File_System_2::clear_file(I_Node* inode) noexcept {
        using namespace stdlib;

        if (inode == nullptr) [[unlikely]]
            return false;

        if (inode->file_data_ptr == nullptr) [[unlikely]]
            return false;

        uint8_t* data_ptr = static_cast<uint8_t*>(inode->file_data_ptr);
        if (data_ptr == nullptr) [[unlikely]]
            return false;

        Memory_Manipulation::set_memory_block(data_ptr, 0, inode->file_byte_size);

        inode->used_data_byte_size = 0;
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

        I_Node* inode = find_file(old_name, old_format);
        if (inode == nullptr) [[unlikely]]
            return false;

        if (find_file(new_name, new_format) != nullptr) [[unlikely]]
            return false;

        String_Manipulation::copy_string(inode->file_name.data(), new_name);
        String_Manipulation::copy_string(inode->file_format.data(), new_format);

        inode->name_hash   = to_fnv1a_hash(new_name);
        inode->format_hash = to_fnv1a_hash(new_format);

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

        I_Node* src_inode = find_file(src_name, src_format);
        if (src_inode == nullptr) [[unlikely]]
            return false;

        if (find_file(dest_name, dest_format) != nullptr) [[unlikely]]
            return false;

        I_Node* dest_inode = create_file(dest_name, 
                                         dest_format, 
                                         src_inode->file_byte_size);
        if (dest_inode == nullptr) [[unlikely]]
            return false;

        uint8_t* src_ptr  = static_cast<uint8_t*>(src_inode->file_data_ptr);
        uint8_t* dest_ptr = static_cast<uint8_t*>(dest_inode->file_data_ptr);
        if (src_ptr == nullptr || dest_ptr == nullptr) [[unlikely]]
            return false;

        Memory_Manipulation::copy_memory_block(dest_ptr,
                                               src_ptr,
                                               src_inode->file_byte_size);

        dest_inode->used_data_byte_size = src_inode->used_data_byte_size;
        return true;
    }

    bool MoleculeOS_File_System_2::read_file(I_Node* inode,
                                             uint8_t* buffer,
                                             const uint32_t buffer_size,
                                             const uint32_t offset,
                                             const uint32_t length) noexcept {
        using namespace stdlib;

        if (inode == nullptr) [[unlikely]]
            return false;

        if (inode->file_data_ptr == nullptr) [[unlikely]]
           return false;

        if (buffer == nullptr) [[unlikely]]
            return false;

        if (offset >= inode->file_byte_size) [[unlikely]]
            return false;

        if (offset + length > inode->file_byte_size) [[unlikely]]
            return false;

        if (buffer_size < length) [[unlikely]]
            return false;

        uint8_t* src_ptr = static_cast<uint8_t*>(inode->file_data_ptr);
        if (src_ptr == nullptr) [[unlikely]]
           return false;

        Memory_Manipulation::copy_memory_block(buffer, 
                                               src_ptr + offset, 
                                               length);

        return true;
    }

    bool MoleculeOS_File_System_2::resize_file_size(I_Node* inode,
                                                    const uint32_t new_size) 
                                                    noexcept {
        if (inode == nullptr) [[unlikely]]
            return false;

        if (inode->file_data_ptr == nullptr) [[unlikely]]
            return false;

        void* new_ptr = heap::Block_Allocator::reallocate(inode->file_data_ptr,
                                                          new_size);
        if (new_ptr == nullptr) [[unlikely]]
            return false;

        inode->file_data_ptr  = new_ptr;
        inode->file_byte_size = new_size;

        if (inode->used_data_byte_size > new_size)
            inode->used_data_byte_size = new_size;

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
