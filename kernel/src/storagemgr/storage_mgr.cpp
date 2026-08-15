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

#include "storagemgr/storage_mgr.hpp"

namespace kernel::storagmgr
{
    bool Storage_Manager::read_or_write_bytes(drivers::ata::Driver_Operations op,
                                              const uint32_t offset,
                                              const uint32_t size,
                                              void* buffer) noexcept {
        using namespace drivers::ata;
        using namespace stdlib;
        using namespace heap;

        if (size == 0) [[unlikely]]
            return false;

        if (buffer == nullptr) [[unlikely]]
            return false;

        const uint32_t start_sector  = offset / SECTOR_SIZE;
        const uint32_t sector_offset = offset % SECTOR_SIZE;
        const uint32_t end_offset    = sector_offset + size;
        const uint32_t sector_count  = (end_offset + SECTOR_SIZE - 1) / 
                                        SECTOR_SIZE;

        void* ptr;
        Block_Allocator::allocate(ptr, sector_count * SECTOR_SIZE);
        uint16_t* sector_buffer = reinterpret_cast<uint16_t*>(ptr);
        if (!sector_buffer) [[unlikely]]
            return false;

        uint8_t* sector_bytes = reinterpret_cast<uint8_t*>(sector_buffer);
        if (op == Driver_Operations::READ) {
            if (!Programmable_Input_Output::run(Driver_Operations::READ,
                                                sector_count,
                                                sector_buffer,
                                                start_sector)) [[unlikely]] {
                Block_Allocator::deallocate(reinterpret_cast<void*>(sector_buffer));
                return false;
            }       

            Memory_Manipulation::copy_memory_block(buffer,
                                                   sector_bytes + sector_offset,
                                                   size);
        }
        else {
            if (!Programmable_Input_Output::run(Driver_Operations::READ,
                                                sector_count,
                                                sector_buffer,
                                                start_sector)) [[unlikely]] {   
                Block_Allocator::deallocate(reinterpret_cast<void*>(sector_buffer));
                return false;
            }

            Memory_Manipulation::copy_memory_block(sector_bytes + sector_offset,
                                                   buffer,
                                                   size);

            if (!Programmable_Input_Output::run(Driver_Operations::WRITE,
                                                sector_count,
                                                sector_buffer,
                                                start_sector)) [[unlikely]] {   
                Block_Allocator::deallocate(reinterpret_cast<void*>(sector_buffer));
                return false;
            }
        }

        Block_Allocator::deallocate(reinterpret_cast<void*>(sector_buffer));
        return true;
    }

    void Storage_Manager::init() noexcept {
        using namespace stdlib;
        using namespace kernel::filesys;

        Filesys_Header header;

        read_or_write_bytes(drivers::ata::Driver_Operations::READ,
                            FILESYS_HEADER_OFFSET,
                            sizeof(header),
                            &header);

        if (String_Manipulation::compare_strings(header.magic.data(), 
                                                 FILESYS_HEADER_MAGIC) != status::EQUAL_TO) {

            String_Manipulation::copy_string(header.magic.data(), 
                                             FILESYS_HEADER_MAGIC);
            header.version                  = MOFS_VERSION;
            header.file_header_count        = FILE_HEADER_TABLE_ENTRYS;
            header.file_header_table_offset = FILE_HEADER_TABLE_OFFSET;
            header.data_offset              = FILESYS_DATA_OFFSET;

            read_or_write_bytes(drivers::ata::Driver_Operations::WRITE,
                                FILESYS_HEADER_OFFSET,
                                sizeof(header),
                                &header);
        }
    }

    bool Storage_Manager::save_filesys() noexcept {
        using namespace stdlib;
        using namespace kernel::filesys;

        Filesys_Header header;
        String_Manipulation::copy_string(header.magic.data(),
                                         FILESYS_HEADER_MAGIC);
        header.version                  = MOFS_VERSION;
        header.file_header_count        = FILE_HEADER_TABLE_ENTRYS;
        header.file_header_table_offset = FILE_HEADER_TABLE_OFFSET;
        header.data_offset              = FILESYS_DATA_OFFSET;
        
        if (!read_or_write_bytes(drivers::ata::Driver_Operations::WRITE,
                                 FILESYS_HEADER_OFFSET,
                                 sizeof(Filesys_Header),
                                 &header)) [[unlikely]]
            return false;

        static stdlib::Array<Serialized_File_Header, 
                             FILE_HEADER_TABLE_ENTRYS> serialized_file_headers;
        serialized_file_headers.fill(Serialized_File_Header{});

        uint32_t current_data_offset = 0;
        for (uint32_t i = 0; i < header.file_header_count; ++i) {
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
                if (!read_or_write_bytes(drivers::ata::Driver_Operations::WRITE,
                                         header.data_offset + current_data_offset,
                                         file_header.file_byte_size,
                                         file_header.file_data_ptr)) [[unlikely]]
                    return false;
            }

            const uint32_t ALIGN_MASK = file_header.used_data_byte_size - 1;

            current_data_offset += file_header.used_data_byte_size;
            current_data_offset = (current_data_offset + ALIGN_MASK) & 
                                   ~ALIGN_MASK;
        }

        const uint32_t file_header_table_size = header.file_header_count * 
                                          sizeof(Serialized_File_Header);
        if (!read_or_write_bytes(drivers::ata::Driver_Operations::WRITE,
                                 header.file_header_table_offset,
                                 file_header_table_size,
                                 serialized_file_headers.data())) [[unlikely]]
            return false;

        return true;
    }

    bool Storage_Manager::load_filesys() noexcept {
        using namespace stdlib;
        using namespace kernel::filesys;
        using namespace heap;

        Filesys_Header header;
        if (!read_or_write_bytes(drivers::ata::Driver_Operations::READ,
                                 FILESYS_HEADER_OFFSET,
                                 sizeof(Filesys_Header),
                                 &header)) [[unlikely]]
            return false;

        if (header.version != MOFS_VERSION) [[unlikely]]
            return false;

        if (String_Manipulation::copy_string(header.magic.data(), 
                                             FILESYS_HEADER_MAGIC) != status::SUCCESS) [[unlikely]]
            return false;

        static stdlib::Array<Serialized_File_Header, 
                             FILE_HEADER_TABLE_ENTRYS> serialized_file_headers;
        serialized_file_headers.fill(Serialized_File_Header{});

        const uint32_t file_header_table_size = header.file_header_count *
                                          sizeof(Serialized_File_Header);
        if (!read_or_write_bytes(drivers::ata::Driver_Operations::READ,
                                 header.file_header_table_offset,
                                 file_header_table_size,
                                 serialized_file_headers.data())) [[unlikely]]
            return false;

        for (uint32_t i = 0; i < header.file_header_count; ++i) {
            Serialized_File_Header& serialized_file_header = serialized_file_headers[i];
            if (serialized_file_header.file_byte_size == 0)
                continue;

            void* data_ptr;
            Block_Allocator::allocate(data_ptr, serialized_file_header.file_byte_size);
            if (!data_ptr) [[unlikely]]
                return false;

            if (!read_or_write_bytes(drivers::ata::Driver_Operations::READ,
                                     header.data_offset + serialized_file_header.file_data_offset,
                                     serialized_file_header.file_byte_size,
                                     data_ptr)) [[unlikely]] {
                Block_Allocator::deallocate(reinterpret_cast<void*>(data_ptr));
                return false;
            }

            File_Header file_header;

            Memory_Manipulation::copy_memory_block(file_header.file_name.data(),
                                                   serialized_file_header.file_name.data(),
                                                   MAX_FILE_NAME_LENGTH + 1);

            Memory_Manipulation::copy_memory_block(file_header.file_format.data(),
                                                   serialized_file_header.file_format.data(),
                                                   MAX_FILE_FORMAT_LENGTH + 1);

            file_header.name_hash           = serialized_file_header.name_hash;
            file_header.format_hash         = serialized_file_header.format_hash;
            file_header.file_byte_size      = serialized_file_header.file_byte_size;
            file_header.used_data_byte_size = serialized_file_header.used_data_byte_size;
            file_header.file_data_ptr       = data_ptr;

            MoleculeOS_File_System_2::set_file_header_entry(file_header, i);
        }

        return true;
    }
} // kernel::storagmgr
