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

#pragma once 

#include "helpers.hpp"
#include <types.hpp>
#include <drivers.hpp>
#include <sal.hpp>
#include <status.hpp>
#include <memory_manip.hpp>

namespace kernel::storagemgr
{
    class Storage_Manager final {
    private:
        static constexpr const char* FILESYS_HEADER_MAGIC = "MOFS";

        static constexpr uint32_t MOFS_VERSION          = 2;
        static constexpr uint32_t FILESYS_HEADER_OFFSET = 0;
        static constexpr uint32_t SECTOR_SIZE           = drivers::ata::SECTOR_SIZE;
        static constexpr uint32_t FILE_HEADER_TABLE_OFFSET = SECTOR_SIZE;
        static constexpr uint32_t FILESYS_DATA_OFFSET      = FILE_HEADER_TABLE_OFFSET + 
                                                             filesys::FILE_HEADER_TABLE_ENTRYS * 
                                                             sizeof(Serialized_File_Header);

        static 
        status_t read_or_write_bytes(_OUT_ void* buffer,
                                     _IN_  drivers::ata::Driver_Operations op,
                                     _IN_  const uint32_t offset, 
                                     _IN_  const uint32_t size) noexcept;

    public:
        static void init() noexcept;

        static bool save_filesys() noexcept;
        static bool load_filesys() noexcept;

        Storage_Manager() noexcept  = default;
        ~Storage_Manager() noexcept = default;
    };
} // namespace kernel::storagemgr
