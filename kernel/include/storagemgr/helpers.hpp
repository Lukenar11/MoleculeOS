#pragma once

#include "filesys/mofs2.hpp"
#include <types.h>
#include <array.hpp>

namespace kernel::storagmgr
{
    struct Filesys_Header final {
        uint32_t magic              = 0;
        uint32_t version            = 0;
        uint32_t inode_count        = 0;
        uint32_t inode_table_offset = 0;
        uint32_t data_offset        = 0;
    };

    struct Serialized_I_Node final {
        runtime::Array<char, filesys::MAX_FILE_NAME_LENGTH + 1> file_name;
        runtime::Array<char, filesys::MAX_FILE_FORMAT_LENGTH + 1> file_format;
        uint32_t name_hash           = 0;
        uint32_t format_hash         = 0;
        uint32_t file_byte_size      = 0;
        uint32_t used_data_byte_size = 0;
        uint32_t file_data_offset    = 0;
    };
} // namespace kernel::storagmgr
