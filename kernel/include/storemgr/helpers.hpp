/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file implements helper structures for the storage manager.

    'Filesys_Header' begins at the first hard-drive sector and 
    includes meta infos for the file system.

    A 'Serialized_File_Header'-table contains the file_header infos which is fetched from the 
    file_headers in RAM.
    The Serialized_File_Header table begins in the second sector of the hard drive,
    immediately after the file-system header.

NOTES:

*/

#pragma once

#include "filesys/mofs2.hpp"
#include <types.hpp>
#include <array.hpp>

namespace kernel::storemgr
{
    struct Filesys_Header final {
        stdlib::Array<char, 5> magic;
        uint32_t version                  = 0;
        uint32_t file_header_count        = 0;
        uint32_t file_header_table_offset = 0;
        uint32_t data_offset              = 0;
    };

    struct Serialized_File_Header final {
        stdlib::Array<char, filesys::MAX_FILE_NAME_LENGTH + 1> file_name;
        stdlib::Array<char, filesys::MAX_FILE_FORMAT_LENGTH + 1> file_format;
        uint32_t name_hash           = 0;
        uint32_t format_hash         = 0;
        uint32_t file_byte_size      = 0;
        uint32_t used_data_byte_size = 0;
        uint32_t file_data_offset    = 0;
    };
} // namespace kernel::storemgr
