/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file implements helper structures for the storage manager.

    'Filesys_Header' begins at the first hard-drive sector and 
    includes meta informations for the file system.

    A 'Serialized_I_Node'-table contains the inode infos which is fetched from the 
    inodes in the RAM.
    The Serialized_I_Node table begins in the second sector of the hard drive,
    immediately after the file-system header.

NOTES:

*/

#pragma once

#include "filesys/mofs2.hpp"
#include <types.h>
#include <array.hpp>

namespace kernel::storagmgr
{
    struct Filesys_Header final {
        runtime::Array<char, 5> magic;
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
