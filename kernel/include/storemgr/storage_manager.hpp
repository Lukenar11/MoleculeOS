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
#include <status.hpp>
#include <sal.hpp>
#include <memory_manip.hpp>

namespace kernel::storemgr
{
    class Storage_Manager final {
    private:
        static constexpr const char* MOFS_HEADER_MAGIC = "MOFS";

        static constexpr uint32_t MOFS_VERSION            = 2;
        static constexpr uint32_t MOFS_HEADER_OFFSET      = 0;
        static constexpr uint32_t SECTOR_SIZE             = drivers::ata::SECTOR_SIZE;
        static constexpr uint32_t FILE_ENTRY_TABLE_OFFSET = SECTOR_SIZE;
        static constexpr uint32_t FILESYS_DATA_OFFSET     = FILE_ENTRY_TABLE_OFFSET + 
                                                            filesys::FILE_TABLE_ENTRYS * 
                                                            sizeof(Stored_File_Entry);
        
        static
        status_t read_bytes(_IN_  const uint32_t start_sector,
                            _IN_  const uint32_t sector_count,
                            _IN_  const uint32_t sector_offset,
                            _IN_  const uint32_t size,
                            _IN_  uint16_t* sector_words,
                            _OUT_ uint16_t* buffer) noexcept;

        static
        status_t write_bytes(_IN_ const uint32_t start_sector,
                             _IN_ const uint32_t sector_count,
                             _IN_ const uint32_t sector_offset,
                             _IN_ const uint32_t size,
                             _IN_ uint16_t* sector_words,
                             _IN_ uint16_t* buffer) noexcept;

        static 
        status_t read_or_write_bytes(_IN_    drivers::ata::Operations op,
                                     _IN_    const uint32_t offset, 
                                     _IN_    const uint32_t size,
                                     _INOUT_ void* buffer) noexcept;

        static
        status_t read_filesystem_header(_OUT_ MOFS_Header& header) noexcept;

        static
        status_t write_filesystem_header(_IN_ MOFS_Header& header) noexcept;
        
        static
        status_t load_single_file(_IN_ const MOFS_Header& header,
                                  _IN_ const Stored_File_Entry& stored,
                                  _IN_ const uint32_t index) noexcept;

        static
        status_t store_single_file(_INOUT_ uint32_t& current_data_offset,
                                   _OUT_   Stored_File_Entry& stored,
                                   _IN_    filesys::File_Entry& file_entry,
                                   _IN_    const MOFS_Header& header) noexcept;

        static
        status_t write_file_entry_table(_OUT_ Stored_File_Entry* table,
                                        _IN_  const MOFS_Header& header) 
                                        noexcept;
    
    public:
        static 
        void init() noexcept;

        static 
        status_t save_filesystem() noexcept;

        static 
        status_t load_filesystem() noexcept;

        Storage_Manager() noexcept  = default;
        ~Storage_Manager() noexcept = default;
    };
} // namespace kernel::storemgr
