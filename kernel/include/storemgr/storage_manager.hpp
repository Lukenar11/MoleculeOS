/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This class implements a storage manager, 
    this manager loads file-system at the boot or reboot phase from the 
    hard-drive into the RAM and saves the file-system from the RAM into the 
    hard-drive at the shutdown or reboot phase.

NOTES:
*/


#pragma once 

#include "helpers.hpp"
#include <types.hpp>
#include <status.hpp>
#include <sal.hpp>
#include <kernel.hpp>
#include <drivers.hpp>
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


        /**
         * @brief Validates all parameters for a byte read or write request.
         *
         * @param buffer_ptr      Pointer to the target or source buffer.
         * @param temp_buffer_ptr Pointer to a temporary sector buffer.
         * @param byte_size       Count of bytes to read or write.
         * @param sector_count    Count of sectors to read or write.
         *
         * @retval `status::NULL_POINTER | status::flags::PARAM_A`
         *          If `buffer` is `nullptr`.
         *
         * @retval `status::NULL_POINTER | status::flags::PARAM_B`
         *          If `temp_buffer` is `nullptr`.
         *
         * @retval `status::INVALID_PARAMETER | status::flags::PARAM_C`
         *          If `byte_size` is `0`.
         *
         * @retval `status::INVALID_PARAMETER | status::flags::PARAM_D`
         *          If `sector_count` is `0`.
         *
         * @retval `status::SUCCESS`
         *          Default case.
         */
        static
        status_t
        validate_byte_reads_and_writes(_IN_ uint16_t* buffer,
                                       _IN_ uint16_t* sector_words,
                                       _IN_ const uint32_t size,
                                       _IN_ const uint32_t sector_count)
                                       noexcept;


        /** 
         * @brief Reads a byte range from the hard drive into the RAM.
         *
         * @param buffer_ptr      Pointer to the destination buffer.
         * @param temp_buffer_ptr Pointer to a temporary sector buffer.
         * @param byte_size       Count of bytes to read.
         * @param sector_count    Count of sectors to read.
         * @param start_sector    LBA address of the start sector.
         * @param sector_offset   Offset inside the start sector.
         * 
         * @retval `status::NULL_POINTER | status::flags::PARAM_A` 
         *          If `buffer` is `nullptr`. 
         * 
         * @retval `status::NULL_POINTER | status::flags::PARAM_B` 
         *          If `temp_buffer` is `nullptr`. 
         * 
         * @retval `status::INVALID_PARAMETER | status::flags::PARAM_C` 
         *          If `byte_size` is `0`. 
         * 
         * @retval `status::INVALID_PARAMETER | status::flags::PARAM_D` 
         *          If `sector_count` is `0`. 
         * 
         * @retval `status::ATA_INVALID_SECTOR_COUNT | status::flags::PARAM_B` 
         *          If `sector_count` is zero or exceeds the allowed maximum.
         * 
         * @retval `status::ATA_INVALID_LBA | status::flags::PARAM_C` 
         *          If the LBA range is invalid.
         * 
         * @retval `status::ATA_ERROR` 
         *          If the hard drive reports an error.
         * 
         * @retval `status::ATA_DEVICE_FAULT` 
         *          If the hard drive sets the DF bit.
         * 
         * @retval `status::ATA_TIMEOUT` 
         *          If no response is received in the timeout period.
         * 
         * @retval `status::SUCCESS` 
         *          Default case. 
         */
        static
        status_t 
        read_bytes(_OUT_ uint16_t* buffer,
                   _IN_  uint16_t* sector_words,
                   _IN_  const uint32_t size,
                   _IN_  const uint32_t sector_count,
                   _IN_  const uint32_t start_sector,
                   _IN_  const uint32_t sector_offset) noexcept;


        /** 
         * @brief Writes a byte range from the RAM into the hard drive.
         *
         * @param buffer_ptr      Pointer to the source buffer.
         * @param temp_buffer_ptr Pointer to a temporary sector buffer.
         * @param byte_size       Count of bytes to write.
         * @param sector_count    Count of sectors to write.
         * @param start_sector    LBA address of the start sector.
         * @param sector_offset   Offset inside the start sector.
         * 
         * @retval `status::NULL_POINTER | status::flags::PARAM_A` 
         *          If `buffer` is `nullptr`. 
         * 
         * @retval `status::NULL_POINTER | status::flags::PARAM_B` 
         *          If `temp_buffer` is `nullptr`. 
         * 
         * @retval `status::INVALID_PARAMETER | status::flags::PARAM_C` 
         *          If `byte_size` is `0`. 
         * 
         * @retval `status::INVALID_PARAMETER | status::flags::PARAM_D` 
         *          If `sector_count` is `0`. 
         * 
         * @retval `status::ATA_INVALID_SECTOR_COUNT | status::flags::PARAM_B` 
         *          If `sector_count` is zero or exceeds the allowed maximum.
         * 
         * @retval `status::ATA_INVALID_LBA | status::flags::PARAM_C` 
         *          If the LBA range is invalid.
         * 
         * @retval `status::ATA_ERROR` 
         *          If the hard drive reports an error.
         * 
         * @retval `status::ATA_DEVICE_FAULT` 
         *          If the hard drive sets the DF bit.
         * 
         * @retval `status::ATA_TIMEOUT` 
         *          If no response is received in the timeout period.
         * 
         * @retval `status::SUCCESS` 
         *          Default case. 
         */
        static
        status_t 
        write_bytes(_IN_ uint16_t* buffer,
                    _IN_ uint16_t* sector_words,
                    _IN_ const uint32_t size,
                    _IN_ const uint32_t sector_count,
                    _IN_ const uint32_t start_sector,
                    _IN_ const uint32_t sector_offset) noexcept;


        /** 
         * @brief Reads or writes a byte range from the hard drive, 
         *        based on a command.
         *
         * @param buffer_ptr Pointer to the target or source buffer.
         * @param byte_size  Count of bytes to read or write.
         * @param offset     Offset in the data area. 
         * @param operation  Read or write operation.
         * 
         * @retval `status::NULL_POINTER | status::flags::PARAM_A` 
         *          If `buffer` is `nullptr`. 
         * 
         * @retval `status::NULL_POINTER | status::flags::PARAM_B` 
         *          If `temp_buffer` is `nullptr`. 
         * 
         * @retval `status::INVALID_PARAMETER | status::flags::PARAM_C` 
         *          If `byte_size` is `0`. 
         * 
         * @retval `status::INVALID_PARAMETER | status::flags::PARAM_D` 
         *          If `sector_count` is `0`. 
         * 
         * @retval `status::ATA_INVALID_SECTOR_COUNT | status::flags::PARAM_B` 
         *          If `sector_count` is zero or exceeds the allowed maximum.
         * 
         * @retval `status::ATA_INVALID_LBA | status::flags::PARAM_C` 
         *          If the LBA range is invalid.
         * 
         * @retval `status::ATA_ERROR` 
         *          If the hard drive reports an error.
         * 
         * @retval `status::ATA_DEVICE_FAULT` 
         *          If the hard drive sets the DF bit.
         * 
         * @retval `status::ATA_TIMEOUT` 
         *          If no response is received in the timeout period.
         * 
         * @retval `status::SUCCESS` 
         *          Default case. 
         */
        static 
        status_t 
        read_or_write_bytes(_INOUT_ void* buffer,
                            _IN_    const uint32_t size,
                            _IN_    const uint32_t offset,
                            _IN_    drivers::ata::Operations operation) 
                            noexcept;


        /** 
         * @brief Reads the filesystem header from the 
         *        first sector of the hard drive.
         * 
         * @param header Header of the filesystem.
         * 
         * @retval `status::FS_VERSION_MISMATCH` 
         *          If the filesystem version is invalid. 
         * 
         * @retval `status::FS_INVALID_FILE_ENTRY` 
         *          If the magic value is invalid. 
         * 
         * @retval `status::NULL_POINTER | status::flags::PARAM_A` 
         *          If `buffer` is `nullptr`. 
         * 
         * @retval `status::NULL_POINTER | status::flags::PARAM_B` 
         *          If `temp_buffer` is `nullptr`. 
         * 
         * @retval `status::INVALID_PARAMETER | status::flags::PARAM_C` 
         *          If `byte_size` is `0`. 
         * 
         * @retval `status::INVALID_PARAMETER | status::flags::PARAM_D` 
         *          If `sector_count` is `0`. 
         * 
         * @retval `status::ATA_INVALID_SECTOR_COUNT | status::flags::PARAM_B` 
         *          If `sector_count` is zero or exceeds the allowed maximum.
         * 
         * @retval `status::ATA_INVALID_LBA | status::flags::PARAM_C` 
         *          If the LBA range is invalid.
         * 
         * @retval `status::ATA_ERROR` 
         *          If the hard drive reports an error.
         * 
         * @retval `status::ATA_DEVICE_FAULT` 
         *          If the hard drive sets the DF bit.
         * 
         * @retval `status::ATA_TIMEOUT` 
         *          If no response is received in the timeout period.
         * 
         * @retval `status::SUCCESS` 
         *          Default case. 
         */
        static
        status_t 
        read_filesystem_header(_OUT_ MOFS_Header& header) noexcept;


        /** 
         * @brief Stores one file from the RAM into the hard drive.
         * 
         * @param header New filesystem header.
         * 
         * @retval `status::FS_VERSION_MISMATCH` 
         *          If the filesystem version is invalid. 
         * 
         * @retval `status::FS_INVALID_FILE_ENTRY` 
         *          If the magic value is invalid. 
         * 
         * @retval `status::NULL_POINTER | status::flags::PARAM_A` 
         *          If `buffer` is `nullptr`. 
         * 
         * @retval `status::NULL_POINTER | status::flags::PARAM_B` 
         *          If `temp_buffer` is `nullptr`. 
         * 
         * @retval `status::INVALID_PARAMETER | status::flags::PARAM_C` 
         *          If `byte_size` is `0`. 
         * 
         * @retval `status::INVALID_PARAMETER | status::flags::PARAM_D` 
         *          If `sector_count` is `0`. 
         * 
         * @retval `status::ATA_INVALID_SECTOR_COUNT | status::flags::PARAM_B` 
         *          If `sector_count` is zero or exceeds the allowed maximum.
         * 
         * @retval `status::ATA_INVALID_LBA | status::flags::PARAM_C` 
         *          If the LBA range is invalid.
         * 
         * @retval `status::ATA_ERROR` 
         *          If the hard drive reports an error.
         * 
         * @retval `status::ATA_DEVICE_FAULT` 
         *          If the hard drive sets the DF bit.
         * 
         * @retval `status::ATA_TIMEOUT` 
         *          If no response is received in the timeout period.
         * 
         * @retval `status::SUCCESS` 
         *          Default case. 
         */
        static
        status_t 
        write_filesystem_header(_IN_ MOFS_Header& header) noexcept;
        

        /** 
         * @brief Loads one file from the hard drive into the RAM.
         * 
         * @param header            Filesystem header. 
         * @param stored_file_entry Stored file entry that contains the 
         *                          file metadata and the on-disk data offset. 
         * @param file_entry_index  Index of target file entry in the file system.
         * 
         * @retval `status::NULL_POINTER | status::flags::PARAM_A` 
         *          If `buffer` is `nullptr`. 
         * 
         * @retval `status::NULL_POINTER | status::flags::PARAM_B` 
         *          If `temp_buffer` is `nullptr`. 
         * 
         * @retval `status::INVALID_PARAMETER | status::flags::PARAM_C` 
         *          If `byte_size` is `0`. 
         * 
         * @retval `status::INVALID_PARAMETER | status::flags::PARAM_D` 
         *          If `sector_count` is `0`. 
         * 
         * @retval `status::ATA_INVALID_SECTOR_COUNT | status::flags::PARAM_B` 
         *          If `sector_count` is zero or exceeds the allowed maximum.
         * 
         * @retval `status::ATA_INVALID_LBA | status::flags::PARAM_C` 
         *          If the LBA range is invalid.
         * 
         * @retval `status::ATA_ERROR` 
         *          If the hard drive reports an error.
         * 
         * @retval `status::ATA_DEVICE_FAULT` 
         *          If the hard drive sets the DF bit.
         * 
         * @retval `status::ATA_TIMEOUT` 
         *          If no response is received in the timeout period.
         * 
         * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
         *          If `byte_size` is `0`.
         * 
         * @retval `status::HEAP_EXHAUSTED`
         *          If the memory pool doesn't have enough free space or 
         *          `byte_size` is to large.
         * 
         * @retval `status::SUCCESS` 
         *          Default case. 
         */
        static
        status_t 
        load_single_file(_IN_ const MOFS_Header& header,
                         _IN_ const Stored_File_Entry& stored,
                         _IN_ const uint32_t index) noexcept;


        /** 
         * @brief Stores one file from the hard drive into the file system,
         *        on the RAM.
         * 
         * @param current_data_offset Current write offset. 
         * @param stored_file_entry   File entry with serialized file metadata. 
         * @param file_entry          Active file entry in RAM. 
         * @param header              Filesystem header.
         * 
         * @retval `status::NULL_POINTER | status::flags::PARAM_A` 
         *          If `buffer` is `nullptr`. 
         * 
         * @retval `status::NULL_POINTER | status::flags::PARAM_B` 
         *          If `temp_buffer` is `nullptr`. 
         * 
         * @retval `status::INVALID_PARAMETER | status::flags::PARAM_C` 
         *          If `byte_size` is `0`. 
         * 
         * @retval `status::INVALID_PARAMETER | status::flags::PARAM_D` 
         *          If `sector_count` is `0`. 
         * 
         * @retval `status::ATA_INVALID_SECTOR_COUNT | status::flags::PARAM_B` 
         *          If `sector_count` is zero or exceeds the allowed maximum.
         * 
         * @retval `status::ATA_INVALID_LBA | status::flags::PARAM_C` 
         *          If the LBA range is invalid.
         * 
         * @retval `status::ATA_ERROR` 
         *          If the hard drive reports an error.
         * 
         * @retval `status::ATA_DEVICE_FAULT` 
         *          If the hard drive sets the DF bit.
         * 
         * @retval `status::ATA_TIMEOUT` 
         *          If no response is received in the timeout period.
         * 
         * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
         *          If `byte_size` is `0`.
         * 
         * @retval `status::HEAP_EXHAUSTED`
         *          If the memory pool doesn't have enough free space or 
         *          `byte_size` is to large.
         * 
         * @retval `status::SUCCESS` 
         *          Default case. 
         */
        static
        status_t 
        store_single_file(_INOUT_ uint32_t& current_data_offset,
                          _OUT_   Stored_File_Entry& stored,
                          _IN_    filesys::File_Entry& file_entry,
                          _IN_    const MOFS_Header& header) noexcept;


        /** 
         * @brief Initializes the file entry table from the file system, 
         *        in the RAM. 
         * 
         * @param entry_table_ptr Pointer to a new table with all file headers from 
         *                        the hard drive. 
         * @param header          Filesystem header. 
         * 
         * @retval `status::NULL_POINTER | status::flags::PARAM_A` 
         *          If `buffer` is `nullptr`. 
         * 
         * @retval `status::NULL_POINTER | status::flags::PARAM_B` 
         *          If `temp_buffer` is `nullptr`. 
         * 
         * @retval `status::INVALID_PARAMETER | status::flags::PARAM_C` 
         *          If `byte_size` is `0`. 
         * 
         * @retval `status::INVALID_PARAMETER | status::flags::PARAM_D` 
         *          If `sector_count` is `0`. 
         * 
         * @retval `status::ATA_INVALID_SECTOR_COUNT | status::flags::PARAM_B` 
         *          If `sector_count` is zero or exceeds the allowed maximum.
         * 
         * @retval `status::ATA_INVALID_LBA | status::flags::PARAM_C` 
         *          If the LBA range is invalid.
         * 
         * @retval `status::ATA_ERROR` 
         *          If the hard drive reports an error.
         * 
         * @retval `status::ATA_DEVICE_FAULT` 
         *          If the hard drive sets the DF bit.
         * 
         * @retval `status::ATA_TIMEOUT` 
         *          If no response is received in the timeout period.
         * 
         * @retval `status::SUCCESS` 
         *          Default case. 
         */
        static
        status_t 
        write_file_entry_table(_OUT_ Stored_File_Entry* table,
                               _IN_  const MOFS_Header& header) noexcept;
    

    public:
        /** 
         * @brief Initializes the storage manager. 
         * 
         * @note This function panics on failure. 
         */
        _API_ 
        static 
        void 
        init() noexcept;


        /** 
         * @brief Saves the filesystem with all his current datas from the 
         *        RAM into the hard drive. 
         * 
         * @retval `status::FS_VERSION_MISMATCH` 
         *          If the filesystem version is invalid. 
         * 
         * @retval `status::FS_INVALID_FILE_ENTRY` 
         *          If the magic value is invalid. 
         * 
         * @retval `status::NULL_POINTER | status::flags::PARAM_A` 
         *          If `buffer` is `nullptr`. 
         * 
         * @retval `status::NULL_POINTER | status::flags::PARAM_B` 
         *          If `temp_buffer` is `nullptr`. 
         * 
         * @retval `status::INVALID_PARAMETER | status::flags::PARAM_C` 
         *          If `byte_size` is `0`. 
         * 
         * @retval `status::INVALID_PARAMETER | status::flags::PARAM_D` 
         *          If `sector_count` is `0`. 
         * 
         * @retval `status::ATA_INVALID_SECTOR_COUNT | status::flags::PARAM_B` 
         *          If `sector_count` is zero or exceeds the allowed maximum.
         * 
         * @retval `status::ATA_INVALID_LBA | status::flags::PARAM_C` 
         *          If the LBA range is invalid.
         * 
         * @retval `status::ATA_ERROR` 
         *          If the hard drive reports an error.
         * 
         * @retval `status::ATA_DEVICE_FAULT` 
         *          If the hard drive sets the DF bit.
         * 
         * @retval `status::ATA_TIMEOUT` 
         *          If no response is received in the timeout period.
         * 
         * @retval `status::SUCCESS` 
         *          Default case. 
         */
        _API_ 
        static 
        status_t 
        save_filesystem() noexcept;


        /** 
         * @brief Stores the filesystem with all his current datas from the 
         *        hard drive into the RAM. 
         * 
         * @retval `status::FS_VERSION_MISMATCH` 
         *          If the filesystem version is invalid. 
         * 
         * @retval `status::FS_INVALID_FILE_ENTRY` 
         *          If the magic value is invalid. 
         * 
         * @retval `status::NULL_POINTER | status::flags::PARAM_A` 
         *          If `buffer` is `nullptr`. 
         * 
         * @retval `status::NULL_POINTER | status::flags::PARAM_B` 
         *          If `temp_buffer` is `nullptr`. 
         * 
         * @retval `status::INVALID_PARAMETER | status::flags::PARAM_C` 
         *          If `byte_size` is `0`. 
         * 
         * @retval `status::INVALID_PARAMETER | status::flags::PARAM_D` 
         *          If `sector_count` is `0`. 
         * 
         * @retval `status::ATA_INVALID_SECTOR_COUNT | status::flags::PARAM_B` 
         *          If `sector_count` is zero or exceeds the allowed maximum.
         * 
         * @retval `status::ATA_INVALID_LBA | status::flags::PARAM_C` 
         *          If the LBA range is invalid.
         * 
         * @retval `status::ATA_ERROR` 
         *          If the hard drive reports an error.
         * 
         * @retval `status::ATA_DEVICE_FAULT` 
         *          If the hard drive sets the DF bit.
         * 
         * @retval `status::ATA_TIMEOUT` 
         *          If no response is received in the timeout period.
         * 
         * @retval `status::SUCCESS` 
         *          Default case. 
         */
        _API_ 
        static 
        status_t 
        load_filesystem() noexcept;

        Storage_Manager() noexcept  = default;
        ~Storage_Manager() noexcept = default;
    };
} // namespace kernel::storemgr
