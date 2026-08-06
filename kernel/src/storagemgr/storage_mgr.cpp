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

        const uint32_t n = Programmable_Input_Output::get_sector_word_size() << 1;

        const uint32_t start_sector  = offset / n;
        const uint32_t sector_offset = offset % n;
        const uint32_t end_offset    = sector_offset + size;
        const uint32_t sector_count  = (end_offset + n - 1) / n;

        uint16_t* sector_buffer = reinterpret_cast<uint16_t*>(
            Block_Allocator::allocate(sector_count * n)
        );
        if (!sector_buffer) [[unlikely]]
            return false;

        if (!Programmable_Input_Output::run(op,
                                            sector_count,
                                            sector_buffer,
                                            start_sector)) [[unlikely]] {
            Block_Allocator::deallocate(reinterpret_cast<void*>(sector_buffer));
            return false;
        }

        uint8_t* sector_bytes = reinterpret_cast<uint8_t*>(sector_buffer);
        if (op == Driver_Operations::READ) {
            Memory_Manipulation::copy_memory_block(buffer,
                                                   sector_bytes + sector_offset,
                                                   size);
        }
        else {
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
        Filesys_Header header;
        header.magic              = 0x4D4F4653;
        header.version            = 2;
        header.inode_count        = 256;
        header.inode_table_offset = sizeof(Filesys_Header);
        header.data_offset        = header.inode_table_offset +
                                        header.inode_count * 
                                        sizeof(Serialized_I_Node);

        read_or_write_bytes(drivers::ata::Driver_Operations::WRITE,
                            0,
                            sizeof(header),
                            &header);
    }

    bool Storage_Manager::save_filesys() noexcept {
        using namespace runtime;
        using namespace kernel::filesys;

        Filesys_Header header;
        header.magic              = 0x4D4F4653; // "MOFS"
        header.version            = 2;
        header.inode_count        = 256;
        header.inode_table_offset = sizeof(Filesys_Header);
        header.data_offset        = header.inode_table_offset +
                                    header.inode_count * 
                                    sizeof(Serialized_I_Node);
        
        if (!read_or_write_bytes(drivers::ata::Driver_Operations::WRITE,
                                 0,
                                 sizeof(Filesys_Header),
                                 &header)) [[unlikely]]
            return false;

        static runtime::Array<Serialized_I_Node, 256> serialized_inodes;
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

            serialized_inode.name_hash        = inode.name_hash;
            serialized_inode.format_hash      = inode.format_hash;
            serialized_inode.file_byte_size   = inode.file_byte_size;
            serialized_inode.file_data_offset = current_data_offset;

            if (inode.file_data_ptr && inode.file_byte_size > 0) {
                if (!read_or_write_bytes(drivers::ata::Driver_Operations::WRITE,
                                         header.data_offset + current_data_offset,
                                         inode.file_byte_size,
                                         inode.file_data_ptr)) [[unlikely]]
                    return false;
            }

            current_data_offset += inode.file_byte_size;
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
                                 0,
                                 sizeof(Filesys_Header),
                                 &header)) [[unlikely]]
            return false;

        if (header.magic != 0x4D4F4653) // "MOFS"
            return false;

        static runtime::Array<Serialized_I_Node, 256> serialized_inodes;
        serialized_inodes.fill(Serialized_I_Node{});

        const uint32_t inode_table_size =
            header.inode_count * sizeof(Serialized_I_Node);

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

            I_Node inode;

            Memory_Manipulation::copy_memory_block(inode.file_name.data(),
                                                   serialized.file_name.data(),
                                                   MAX_FILE_NAME_LENGTH + 1);

            Memory_Manipulation::copy_memory_block(inode.file_format.data(),
                                                   serialized.file_format.data(),
                                                   MAX_FILE_FORMAT_LENGTH + 1);

            inode.name_hash      = serialized.name_hash;
            inode.format_hash    = serialized.format_hash;
            inode.file_byte_size = serialized.file_byte_size;
            inode.file_data_ptr  = data_ptr;

            MoleculeOS_File_System_2::set_inode_entry(inode, i);
        }

        return true;
    }
} // kernel::storagmgr
