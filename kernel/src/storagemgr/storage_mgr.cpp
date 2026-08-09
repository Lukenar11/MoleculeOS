#include "storagemgr/storage_mgr.hpp"

namespace kernel::storagmgr
{
    bool Storage_Manager::read_or_write_bytes(drivers::ata::Driver_Operations op,
                                              const uint32_t offset,
                                              const uint32_t size,
                                              void* buffer) noexcept {
        using namespace drivers::ata;
        using namespace runtime;
        using namespace heap;

        const uint32_t start_sector  = offset / SECTOR_SIZE;
        const uint32_t sector_offset = offset % SECTOR_SIZE;
        const uint32_t end_offset    = sector_offset + size;
        const uint32_t sector_count  = (end_offset + SECTOR_SIZE - 1) / 
                                        SECTOR_SIZE;

        uint16_t* sector_buffer = reinterpret_cast<uint16_t*>(
            Block_Allocator::allocate(sector_count * SECTOR_SIZE)
        );
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
        using namespace runtime;
        using namespace kernel::filesys;

        Filesys_Header header;
        String_Manipulation::copy_string(header.magic.data(), 
                                         FILESYS_HEADER_MAGIC);
        header.version            = MOFS_VERSION;
        header.inode_count        = INODE_TABLE_ENTRYS;
        header.inode_table_offset = INODE_TABLE_OFFSET;
        header.data_offset        = FILESYS_DATA_OFFSET;

        read_or_write_bytes(drivers::ata::Driver_Operations::WRITE,
                            FILESYS_HEADER_OFFSET,
                            sizeof(header),
                            &header);
    }

    bool Storage_Manager::save_filesys() noexcept {
        using namespace runtime;
        using namespace kernel::filesys;

        Filesys_Header header;
        String_Manipulation::copy_string(header.magic.data(),
                                         FILESYS_HEADER_MAGIC);
        header.version            = MOFS_VERSION;
        header.inode_count        = INODE_TABLE_ENTRYS;
        header.inode_table_offset = INODE_TABLE_OFFSET;
        header.data_offset        = FILESYS_DATA_OFFSET;
        
        if (!read_or_write_bytes(drivers::ata::Driver_Operations::WRITE,
                                 FILESYS_HEADER_OFFSET,
                                 sizeof(Filesys_Header),
                                 &header)) [[unlikely]]
            return false;

        static runtime::Array<Serialized_I_Node, 
                              INODE_TABLE_ENTRYS> serialized_inodes;
        serialized_inodes.fill(Serialized_I_Node{});

        uint32_t current_data_offset = 0;
        for (uint32_t i = 0; i < header.inode_count; ++i) {
            I_Node& inode = MoleculeOS_File_System_2::get_inode_entry(i);
            if (!inode.file_data_ptr || inode.file_byte_size == 0)
                continue;

            Serialized_I_Node& serialized_inode = serialized_inodes[i];

            Memory_Manipulation::copy_memory_block(serialized_inode.file_name.data(),
                                                   inode.file_name.data(),
                                                   MAX_FILE_NAME_LENGTH + 1);

            Memory_Manipulation::copy_memory_block(serialized_inode.file_format.data(),
                                                   inode.file_format.data(),
                                                   MAX_FILE_FORMAT_LENGTH + 1);

            serialized_inode.name_hash           = inode.name_hash;
            serialized_inode.format_hash         = inode.format_hash;
            serialized_inode.file_byte_size      = inode.file_byte_size;
            serialized_inode.used_data_byte_size = inode.used_data_byte_size;
            serialized_inode.file_data_offset    = current_data_offset;

            if (inode.file_data_ptr && inode.file_byte_size > 0) {
                if (!read_or_write_bytes(drivers::ata::Driver_Operations::WRITE,
                                         header.data_offset + current_data_offset,
                                         inode.file_byte_size,
                                         inode.file_data_ptr)) [[unlikely]]
                    return false;
            }

            const uint32_t ALIGN_MASK = inode.used_data_byte_size - 1;

            current_data_offset += inode.used_data_byte_size;
            current_data_offset = (current_data_offset + ALIGN_MASK) & 
                                   ~ALIGN_MASK;
        }

        const uint32_t inode_table_size = header.inode_count * 
                                          sizeof(Serialized_I_Node);
        if (!read_or_write_bytes(drivers::ata::Driver_Operations::WRITE,
                                 header.inode_table_offset,
                                 inode_table_size,
                                 serialized_inodes.data())) [[unlikely]]
            return false;

        return true;
    }

    bool Storage_Manager::load_filesys() noexcept {
        using namespace runtime;
        using namespace kernel::filesys;
        using namespace heap;

        Filesys_Header header;
        if (!read_or_write_bytes(drivers::ata::Driver_Operations::READ,
                                 FILESYS_HEADER_OFFSET,
                                 sizeof(Filesys_Header),
                                 &header)) [[unlikely]]
            return false;

        if (String_Manipulation::compare_strings(header.magic.data(), 
                                                 FILESYS_HEADER_MAGIC) != 0) [[unlikely]]
            return false;

        static runtime::Array<Serialized_I_Node, INODE_TABLE_ENTRYS> serialized_inodes;
        serialized_inodes.fill(Serialized_I_Node{});

        const uint32_t inode_table_size = header.inode_count *
                                          sizeof(Serialized_I_Node);
        if (!read_or_write_bytes(drivers::ata::Driver_Operations::READ,
                                 header.inode_table_offset,
                                 inode_table_size,
                                 serialized_inodes.data())) [[unlikely]]
            return false;

        for (uint32_t i = 0; i < header.inode_count; ++i) {
            Serialized_I_Node& serialized = serialized_inodes[i];
            if (serialized.file_byte_size == 0)
                continue;

            void* data_ptr = Block_Allocator::allocate(serialized.file_byte_size);
            if (!data_ptr) [[unlikely]]
                return false;

            if (!read_or_write_bytes(drivers::ata::Driver_Operations::READ,
                                     header.data_offset + serialized.file_data_offset,
                                     serialized.file_byte_size,
                                     data_ptr)) [[unlikely]] {
                Block_Allocator::deallocate(reinterpret_cast<void*>(data_ptr));
                return false;
            }

            static I_Node inode;

            Memory_Manipulation::copy_memory_block(inode.file_name.data(),
                                                   serialized.file_name.data(),
                                                   MAX_FILE_NAME_LENGTH + 1);

            Memory_Manipulation::copy_memory_block(inode.file_format.data(),
                                                   serialized.file_format.data(),
                                                   MAX_FILE_FORMAT_LENGTH + 1);

            inode.name_hash           = serialized.name_hash;
            inode.format_hash         = serialized.format_hash;
            inode.file_byte_size      = serialized.file_byte_size;
            inode.used_data_byte_size = serialized.used_data_byte_size;
            inode.file_data_ptr       = data_ptr;

            MoleculeOS_File_System_2::set_inode_entry(inode, i);
        }

        return true;
    }
} // kernel::storagmgr
