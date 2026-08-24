/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This class implements a storage manager, 
    this manager loads file-system at the boot or reboot phase from the hard-drive into the RAM and
    saves the file-system from the RAM into the hard-drive at the shutdown or reboot phase.

NOTES:
*/

#include "storemgr/storage_manager.hpp"

namespace kernel::storemgr
{
    status_t Storage_Manager::read_bytes(_IN_  const uint32_t start_sector,
                                         _IN_  const uint32_t sector_count,
                                         _IN_  const uint32_t sector_offset,
                                         _IN_  const uint32_t size,
                                         _IN_  uint16_t* sector_words,
                                         _OUT_ uint16_t* buffer) noexcept {
        using namespace drivers;
        using namespace stdlib;

        status_t status;
        void* dest_ptr = sector_words + sector_offset;

        if (size == 0 || sector_count == 0) [[unlikely]] {
            status = status::INVALID_PARAMETER;
            goto cleanup;
        }

        if (reinterpret_cast<uint32_t>(sector_words) <
            ata::SECTOR_WORD_SIZE) [[unlikely]] {
            status = status::INVALID_PARAMETER;
            goto cleanup;
        }

        if (!buffer) [[unlikely]] {
            status = status::NULL_POINTER;
            goto cleanup;
        }

        if (!ata::Programmable_Input_Output::run(ata::Operations::READ,
                                                 sector_count,
                                                 sector_words,
                                                 start_sector)) [[unlikely]] {
            status = status::FAIL;
            goto cleanup;
        }

        status = stdlib::Memory_Manipulation::copy_memory_block(buffer,
                                                                dest_ptr,
                                                                size);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        status = status::SUCCESS;

    cleanup:
        return status;
    }

    status_t Storage_Manager::write_bytes(_IN_ const uint32_t start_sector,
                                          _IN_ const uint32_t sector_count,
                                          _IN_ const uint32_t sector_offset,
                                          _IN_ const uint32_t size,
                                          _IN_ uint16_t* sector_words,
                                          _IN_ uint16_t* buffer) noexcept {
        using namespace drivers;
                                        
        status_t status;
        void* dest_ptr = sector_words + sector_offset;

        if (size == 0 || sector_count == 0) [[unlikely]] {
            status = status::INVALID_PARAMETER;
            goto cleanup;
        }

        if (reinterpret_cast<uint32_t>(sector_words) < 
            ata::SECTOR_WORD_SIZE) [[unlikely]] {
            status = status::INVALID_PARAMETER;
            goto cleanup;
        }

        if (!buffer) [[unlikely]] {
            status = status::NULL_POINTER;
            goto cleanup;
        }
                                        
        if (!ata::Programmable_Input_Output::run(ata::Operations::READ,
                                                 sector_count,
                                                 sector_words,
                                                 start_sector)) [[unlikely]] {
            status = status::FAIL;
            goto cleanup;
        }
    
        status = stdlib::Memory_Manipulation::copy_memory_block(dest_ptr,
                                                                buffer,
                                                                size);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }
    
        if (!ata::Programmable_Input_Output::run(ata::Operations::WRITE,
                                                 sector_count,
                                                 sector_words,
                                                 start_sector)) [[unlikely]] {
            status = status::FAIL;
            goto cleanup;
        }
    
        status = status::SUCCESS;
    
    cleanup:
        return status;
    }

    status_t Storage_Manager::read_or_write_bytes(_IN_    drivers::ata::Operations op,
                                                  _IN_    const uint32_t offset,
                                                  _IN_    const uint32_t size,
                                                  _INOUT_ void* buffer) noexcept {
        using namespace drivers;

        status_t status;
        uint16_t* sector_buffer;
        void* ptr = nullptr;

        const uint32_t start_sector    = offset / ata::SECTOR_SIZE;
        const uint32_t sector_offset   = offset % ata::SECTOR_SIZE;
        const uint32_t end_offset      = sector_offset + size;
        const uint32_t sector_count    = (end_offset + ata::SECTOR_SIZE - 1) /
                                          ata::SECTOR_SIZE;

        const uint32_t allocation_size = sector_count * ata::SECTOR_SIZE;

        if (size == 0) [[unlikely]] {
            status = status::INVALID_PARAMETER;
            goto cleanup;
        }

        if (!buffer) [[unlikely]] {
            status = status::NULL_POINTER;
            goto cleanup;
        }

        status = heap::Block_Allocator::allocate(ptr, allocation_size);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        sector_buffer = reinterpret_cast<uint16_t*>(ptr);

        if (op == ata::Operations::READ) {
            status = read_bytes(start_sector,
                                sector_count,
                                sector_offset,
                                size,
                                sector_buffer,
                                reinterpret_cast<uint16_t*>(buffer));
        }
        else {
            status = write_bytes(start_sector,
                                 sector_count,
                                 sector_offset,
                                 size,
                                 sector_buffer,
                                 reinterpret_cast<uint16_t*>(buffer));
        }

        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        status = status::SUCCESS;

    cleanup:
        heap::Block_Allocator::deallocate(ptr);

        return status;
    }

    status_t Storage_Manager::read_filesystem_header(_OUT_ Filesys_Header& header) 
                                                     noexcept {
        using namespace drivers;

        status_t status;

        status = read_or_write_bytes(ata::Operations::READ,
                                     FILESYS_HEADER_OFFSET,
                                     sizeof(Filesys_Header),
                                     &header);
        if (status != status::SUCCESS) [[unlikely]] {
            status = status::FAIL;
            goto cleanup;
        }

        if (header.version != MOFS_VERSION) [[unlikely]] {
            status = status::FS_VERSION_MISMATCH;
            goto cleanup;
        }

        if (stdlib::String_Manipulation::copy_string(header.magic.data(),
                                                     FILESYS_HEADER_MAGIC) 
            != status::SUCCESS) [[unlikely]] {
            status = status::FS_INVALID_FILE_HEADER;
            goto cleanup;
        }

        status = status::SUCCESS;

    cleanup:
        return status;
    }

    status_t Storage_Manager::load_single_file(_IN_ const Filesys_Header& header,
                                               _IN_ const Serialized_File_Header& serialized,
                                               _IN_ const uint32_t index) noexcept {
        using namespace filesys;
        using namespace drivers;

        status_t status;
        void* data_ptr;
        uint32_t file_offset;
        File_Header file_header;

        const uint32_t name_length   = filesys::MAX_FILE_NAME_LENGTH + 1;
        const uint32_t format_length = filesys::MAX_FILE_FORMAT_LENGTH + 1;

        if (serialized.file_byte_size == 0) [[unlikely]] {
            status = status::SUCCESS;
            goto cleanup;
        }

        status = heap::Block_Allocator::allocate(data_ptr, 
                                                 serialized.file_byte_size);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        file_offset = header.data_offset + serialized.file_data_offset;

        status = read_or_write_bytes(ata::Operations::READ,
                                     file_offset,
                                     serialized.file_byte_size,
                                     data_ptr);
        if (status != status::SUCCESS) [[unlikely]] {
            heap::Block_Allocator::deallocate(data_ptr);
            goto cleanup;
        }

        status = stdlib::Memory_Manipulation::copy_memory_block(file_header.file_name.data(),
                                                                serialized.file_name.data(),
                                                                name_length);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        status = stdlib::Memory_Manipulation::copy_memory_block(file_header.file_format.data(),
                                                                serialized.file_format.data(),
                                                                format_length);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        file_header.name_hash           = serialized.name_hash;
        file_header.format_hash         = serialized.format_hash;
        file_header.file_byte_size      = serialized.file_byte_size;
        file_header.used_data_byte_size = serialized.used_data_byte_size;
        file_header.file_data_ptr       = data_ptr;

        filesys::MoleculeOS_File_System_2::set_file_header_entry(file_header, index);

        status = status::SUCCESS;

    cleanup:
        return status;
    }

    void Storage_Manager::init() noexcept {
        using namespace drivers;
        using namespace filesys;

        Filesys_Header header;

        read_or_write_bytes(ata::Operations::READ,
                            FILESYS_HEADER_OFFSET,
                            sizeof(header),
                            &header);

        if (stdlib::String_Manipulation::compare_strings(header.magic.data(), 
                                                         FILESYS_HEADER_MAGIC)
            != status::EQUAL_TO) {
            stdlib::String_Manipulation::copy_string(header.magic.data(), 
                                                     FILESYS_HEADER_MAGIC);
            header.version                  = MOFS_VERSION;
            header.file_header_count        = FILE_HEADER_TABLE_ENTRYS;
            header.file_header_table_offset = FILE_HEADER_TABLE_OFFSET;
            header.data_offset              = FILESYS_DATA_OFFSET;

            read_or_write_bytes(ata::Operations::WRITE,
                                FILESYS_HEADER_OFFSET,
                                sizeof(header),
                                &header);
        }
    }

    bool Storage_Manager::save_filesystem() noexcept {
        using namespace stdlib;
        using namespace filesys;
        using namespace drivers;

        Filesys_Header header;
        String_Manipulation::copy_string(header.magic.data(),
                                         FILESYS_HEADER_MAGIC);
        header.version                  = MOFS_VERSION;
        header.file_header_count        = FILE_HEADER_TABLE_ENTRYS;
        header.file_header_table_offset = FILE_HEADER_TABLE_OFFSET;
        header.data_offset              = FILESYS_DATA_OFFSET;
        
        if (read_or_write_bytes(ata::Operations::WRITE,
                                FILESYS_HEADER_OFFSET,
                                sizeof(Filesys_Header),
                                &header)
            != status::SUCCESS) [[unlikely]] {
            return false;
        }

        static stdlib::Array<Serialized_File_Header, 
                             FILE_HEADER_TABLE_ENTRYS> serialized_file_headers;
        serialized_file_headers.fill(Serialized_File_Header{});

        uint32_t current_data_offset = 0;
        for (uint32_t i = 0; i < header.file_header_count; ++i) [[likely]] {
            File_Header& file_header = MoleculeOS_File_System_2::get_file_header_entry(i);
            if (!file_header.file_data_ptr || file_header.file_byte_size == 0)
                continue;

            Serialized_File_Header& serialized_file_header = serialized_file_headers[i];

            Memory_Manipulation::copy_memory_block(serialized_file_header.file_name.data(),
                                                   file_header.file_name.data(),
                                                   MAX_FILE_NAME_LENGTH + 1);

            Memory_Manipulation::copy_memory_block(serialized_file_header.file_format.data(),
                                                   file_header.file_format.data(),
                                                   MAX_FILE_FORMAT_LENGTH + 1);

            serialized_file_header.name_hash           = file_header.name_hash;
            serialized_file_header.format_hash         = file_header.format_hash;
            serialized_file_header.file_byte_size      = file_header.file_byte_size;
            serialized_file_header.used_data_byte_size = file_header.used_data_byte_size;
            serialized_file_header.file_data_offset    = current_data_offset;

            if (file_header.file_data_ptr && file_header.file_byte_size > 0) {
                if (read_or_write_bytes(ata::Operations::WRITE,
                                        header.data_offset + current_data_offset,
                                        file_header.file_byte_size,
                                        file_header.file_data_ptr) 
                    != status::SUCCESS) [[unlikely]] {
                    return false;
                }
            }

            const uint32_t ALIGN_MASK = file_header.used_data_byte_size - 1;

            current_data_offset += file_header.used_data_byte_size;
            current_data_offset = (current_data_offset + ALIGN_MASK) & 
                                   ~ALIGN_MASK;
        }

        const uint32_t file_header_table_size = header.file_header_count * 
                                          sizeof(Serialized_File_Header);
        if (read_or_write_bytes(ata::Operations::WRITE,
                                header.file_header_table_offset,
                                file_header_table_size,
                                serialized_file_headers.data()) 
        != status::SUCCESS) [[unlikely]] {
            return false;
        }

        return true;
    }

    status_t Storage_Manager::load_filesystem() noexcept {
        using namespace drivers;
        
        status_t status;
        Filesys_Header header;
        stdlib::Array<Serialized_File_Header, 
                      filesys::FILE_HEADER_TABLE_ENTRYS> serialized_file_headers;
        uint32_t table_size;

        status = read_filesystem_header(header);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        table_size = header.file_header_count * sizeof(Serialized_File_Header);

        status = read_or_write_bytes(ata::Operations::READ,
                                     header.file_header_table_offset,
                                     table_size,
                                     serialized_file_headers.data());
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        for (uint32_t i = 0; i < header.file_header_count; ++i) [[likely]] {
            status = load_single_file(header, serialized_file_headers[i], i);
            if (status != status::SUCCESS) [[unlikely]] {
                goto cleanup;
            }
        }

        status = status::SUCCESS;

    cleanup:
        return status;
    }
} // kernel::storemgr
