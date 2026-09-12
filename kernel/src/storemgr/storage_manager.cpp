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


#include "storemgr/storage_manager.hpp"


namespace kernel::storemgr
{
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
    status_t
    Storage_Manager::validate_byte_reads_and_writes(_IN_ uint16_t* buffer_ptr,
                                                    _IN_ uint16_t* temp_buffer_ptr,
                                                    _IN_ const uint32_t byte_size,
                                                    _IN_ const uint32_t sector_count) 
                                                    noexcept {
        status_t status;

        if (!buffer_ptr) [[unlikely]] {
            status = status::NULL_POINTER | status::flags::PARAM_A;
            goto cleanup;
        }

        if (!temp_buffer_ptr) [[unlikely]] {
            status = status::NULL_POINTER | status::flags::PARAM_B;
            goto cleanup;
        }

        if (byte_size == 0) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_C;
            goto cleanup;
        }

        if (sector_count == 0) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_D;
            goto cleanup;
        }

        status = status::SUCCESS;

    cleanup:
        return status;
    }

    
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
    status_t 
    Storage_Manager::read_bytes(_OUT_ uint16_t* buffer_ptr,
                                _IN_  uint16_t* temp_buffer_ptr,
                                _IN_  const uint32_t byte_size,
                                _IN_  const uint32_t sector_count,
                                _IN_  const uint32_t start_sector,
                                _IN_  const uint32_t sector_offset) noexcept {
        using namespace drivers;
        using namespace stdlib;

        status_t status;
        uint8_t* dest_ptr;

        status = validate_byte_reads_and_writes(buffer_ptr,
                                                temp_buffer_ptr,
                                                byte_size,
                                                sector_count);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        status = ata::Programmable_Input_Output::run(temp_buffer_ptr,
                                                     start_sector,
                                                     sector_count,
                                                     ata::Operations::READ);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        dest_ptr = reinterpret_cast<uint8_t*>(temp_buffer_ptr) + sector_offset;

        status = stdlib::Memory_Manipulation::copy_memory_block(buffer_ptr,
                                                                dest_ptr,
                                                                byte_size);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        status = status::SUCCESS;

    cleanup:
        return status;
    }


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
    status_t 
    Storage_Manager::write_bytes(_IN_ uint16_t* buffer_ptr,
                                 _IN_ uint16_t* temp_buffer_ptr,
                                 _IN_ const uint32_t byte_size,
                                 _IN_ const uint32_t sector_count,
                                 _IN_ const uint32_t start_sector,
                                 _IN_ const uint32_t sector_offset) noexcept {
        using namespace drivers;
                                        
        status_t status;
        uint8_t* dest_ptr;

        status = validate_byte_reads_and_writes(buffer_ptr,
                                                temp_buffer_ptr,
                                                byte_size,
                                                sector_count);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }
                             
        status = ata::Programmable_Input_Output::run(temp_buffer_ptr,
                                                     start_sector,
                                                     sector_count,
                                                     ata::Operations::READ);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }
    
        dest_ptr = reinterpret_cast<uint8_t*>(temp_buffer_ptr) + sector_offset;

        status = stdlib::Memory_Manipulation::copy_memory_block(dest_ptr,
                                                                buffer_ptr,
                                                                byte_size);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }
    
        status = ata::Programmable_Input_Output::run(temp_buffer_ptr,
                                                     start_sector,
                                                     sector_count,
                                                     ata::Operations::WRITE);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }
    
        status = status::SUCCESS;
    
    cleanup:
        return status;
    }


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
    status_t
    Storage_Manager::read_or_write_bytes(_INOUT_ void* buffer_ptr,
                                         _IN_    const uint32_t byte_size,
                                         _IN_    const uint32_t offset,
                                         _IN_    drivers::ata::Operations operation)
                                         noexcept {
        using namespace drivers;

        status_t status;
        uint16_t* sector_buffer;
        void* ptr = nullptr;
        uint32_t start_sector;
        uint32_t sector_offset;
        uint32_t end_offset;
        uint32_t sector_count;
        uint32_t allocation_size;

        if (!buffer_ptr) [[unlikely]] {
            status = status::NULL_POINTER | status::flags::PARAM_A;
            goto cleanup;
        }

        if (byte_size == 0) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_B;
            goto cleanup;
        }

        start_sector  = offset / drivers::ata::SECTOR_SIZE;
        sector_offset = offset % drivers::ata::SECTOR_SIZE;

        if (byte_size > (UINT32_MAX - sector_offset)) [[unlikely]] {
            status = status::BUFFER_OVERFLOW;
            goto cleanup;
        }

        end_offset   = sector_offset + byte_size;
        sector_count = (end_offset + drivers::ata::SECTOR_SIZE - 1) / 
                        drivers::ata::SECTOR_SIZE;

        if (sector_count == 0) [[unlikely]] {
            status = status::INVALID_PARAMETER | status::flags::PARAM_C;
            goto cleanup;
        }

        allocation_size = sector_count * ata::SECTOR_SIZE;

        if (allocation_size / ata::SECTOR_SIZE != sector_count) [[unlikely]] {
            status = status::BUFFER_OVERFLOW;
            goto cleanup;
        }

        status = heap::Block_Allocator::allocate(ptr, allocation_size);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        sector_buffer = reinterpret_cast<uint16_t*>(ptr);

        if (operation == ata::Operations::READ) {
            status = read_bytes(reinterpret_cast<uint16_t*>(buffer_ptr),
                                sector_buffer,
                                byte_size,
                                sector_count,
                                start_sector,
                                sector_offset);
        }
        else {
            status = write_bytes(reinterpret_cast<uint16_t*>(buffer_ptr),
                                 sector_buffer,
                                 byte_size,
                                 sector_count,
                                 start_sector,
                                 sector_offset);
        }

    cleanup:
        heap::Block_Allocator::deallocate(ptr);

        return status;
    }


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
    status_t 
    Storage_Manager::read_filesystem_header(_OUT_ MOFS_Header& header)
                                            noexcept {
        using namespace drivers;

        status_t status;

        status = read_or_write_bytes(&header,
                                     sizeof(MOFS_Header),
                                     MOFS_HEADER_OFFSET,
                                     ata::Operations::READ);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        if (header.version != MOFS_VERSION) [[unlikely]] {
            status = status::FS_VERSION_MISMATCH;
            goto cleanup;
        }

        if (stdlib::String_Manipulation::compare_strings(header.magic.data(),
                                                         MOFS_HEADER_MAGIC)
            != status::EQUAL_TO) [[unlikely]] {
            status = status::FS_INVALID_FILE_ENTRY;
            goto cleanup;
        }

        status = status::SUCCESS;

    cleanup:
        return status;
    }


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
    status_t 
    Storage_Manager::write_filesystem_header(_IN_ MOFS_Header& header) 
                                             noexcept {
        using namespace filesys;
        using namespace drivers;

        status_t status;

        status = stdlib::String_Manipulation::copy_string(header.magic.data(),
                                                          MOFS_HEADER_MAGIC);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        header.version                 = MOFS_VERSION;
        header.file_entry_count        = FILE_TABLE_ENTRYS;
        header.file_entry_table_offset = FILE_ENTRY_TABLE_OFFSET;
        header.data_offset             = FILESYS_DATA_OFFSET;

        status = read_or_write_bytes(&header,
                                     sizeof(MOFS_Header),
                                     MOFS_HEADER_OFFSET,
                                     ata::Operations::WRITE);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        status = status::SUCCESS;

    cleanup:
        return status;
    }


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
    status_t 
    Storage_Manager::load_single_file(_IN_ const MOFS_Header& header,
                                      _IN_ const Stored_File_Entry& stored_file_entry,
                                      _IN_ const uint32_t file_entry_index) 
                                      noexcept {
        using namespace drivers;
        using namespace stdlib;

        status_t status;
        uint32_t file_offset;
        filesys::File_Entry file_entry;
        void* data_ptr = nullptr;

        const uint32_t name_length   = filesys::MAX_FILE_NAME_LENGTH + 1;
        const uint32_t format_length = filesys::MAX_FILE_FORMAT_LENGTH + 1;

        if (stored_file_entry.file_byte_size == 0) [[unlikely]] {
            status = status::SUCCESS;
            goto done;
        }

        status = heap::Block_Allocator::allocate(data_ptr, 
                                                 stored_file_entry.file_byte_size);
        if (status != status::SUCCESS) [[unlikely]] {
            goto done;
        }

        file_offset = header.data_offset + stored_file_entry.file_data_offset;

        status = read_or_write_bytes(data_ptr,
                                     stored_file_entry.file_byte_size,
                                     file_offset,
                                     ata::Operations::READ);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        status = Memory_Manipulation::copy_memory_block(file_entry.file_name.data(),
                                                        stored_file_entry.file_name.data(),
                                                        name_length);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        status = Memory_Manipulation::copy_memory_block(file_entry.file_format.data(),
                                                        stored_file_entry.file_format.data(),
                                                        format_length);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        file_entry.name_hash           = stored_file_entry.name_hash;
        file_entry.format_hash         = stored_file_entry.format_hash;
        file_entry.file_byte_size      = stored_file_entry.file_byte_size;
        file_entry.used_data_byte_size = stored_file_entry.used_data_byte_size;
        file_entry.file_data_ptr       = data_ptr;

        filesys::MoleculeOS_File_System_2::set_file_entry(file_entry, 
                                                          file_entry_index);

        status = status::SUCCESS;

        goto done;

    cleanup:
        heap::Block_Allocator::deallocate(data_ptr);

    done:
        return status;
    }


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
    status_t 
    Storage_Manager::store_single_file(_INOUT_ uint32_t& current_data_offset,
                                       _OUT_   Stored_File_Entry& stored_file_entry,
                                       _IN_    filesys::File_Entry& file_entry,
                                       _IN_    const MOFS_Header& header) 
                                       noexcept {
        using namespace filesys;
        using namespace drivers;
        using namespace stdlib;

        status_t status;

        if (!file_entry.file_data_ptr || 
            file_entry.file_byte_size == 0) [[unlikely]] {
            status = status::SUCCESS;
            goto cleanup;
        }

        status = Memory_Manipulation::copy_memory_block(stored_file_entry.file_name.data(),
                                                        file_entry.file_name.data(),
                                                        MAX_FILE_NAME_LENGTH + 1);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        status = Memory_Manipulation::copy_memory_block(stored_file_entry.file_format.data(),
                                                        file_entry.file_format.data(),
                                                        MAX_FILE_FORMAT_LENGTH + 1);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        stored_file_entry.name_hash           = file_entry.name_hash;
        stored_file_entry.format_hash         = file_entry.format_hash;
        stored_file_entry.file_byte_size      = file_entry.file_byte_size;
        stored_file_entry.used_data_byte_size = file_entry.used_data_byte_size;
        stored_file_entry.file_data_offset    = current_data_offset;

        status = read_or_write_bytes(file_entry.file_data_ptr,
                                     file_entry.file_byte_size,
                                     header.data_offset + current_data_offset,
                                     ata::Operations::WRITE);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        current_data_offset += file_entry.file_byte_size;

        status = status::SUCCESS;

    cleanup:
        return status;
    }


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
    status_t 
    Storage_Manager::write_file_entry_table(_OUT_ Stored_File_Entry* entry_table_ptr,
                                            _IN_  const MOFS_Header& header)
                                            noexcept {
        using namespace drivers;
        using namespace filesys;

        status_t status;
        const uint32_t file_entry_table_size = header.file_entry_count * 
                                               sizeof(Stored_File_Entry);

        status = read_or_write_bytes(entry_table_ptr,
                                     file_entry_table_size,
                                     header.file_entry_table_offset,
                                     ata::Operations::WRITE);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        status = status::SUCCESS;

    cleanup:
        return status;
    }


    /** 
     * @brief Initializes the storage manager. 
     * 
     * @note This function panics on failure. 
     */
    _API_ 
    void 
    Storage_Manager::init() noexcept {
        using namespace drivers;
        using namespace filesys;

        MOFS_Header header;
        status_t status;

        sys::disable_interrupts();

        status = read_or_write_bytes(&header,
                                     sizeof(header),
                                     MOFS_HEADER_OFFSET,
                                     ata::Operations::READ);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        if (stdlib::String_Manipulation::compare_strings(header.magic.data(), 
                                                         MOFS_HEADER_MAGIC)
            != status::EQUAL_TO) [[unlikely]] {
            stdlib::String_Manipulation::copy_string(header.magic.data(), 
                                                     MOFS_HEADER_MAGIC);
            header.version                 = MOFS_VERSION;
            header.file_entry_count        = FILE_TABLE_ENTRYS;
            header.file_entry_table_offset = FILE_ENTRY_TABLE_OFFSET;
            header.data_offset             = FILESYS_DATA_OFFSET;

            status = read_or_write_bytes(&header,
                                         sizeof(header),
                                         MOFS_HEADER_OFFSET,
                                         ata::Operations::WRITE);
            if (status != status::SUCCESS) [[unlikely]] {
                goto cleanup;
            }
        }

        goto done;

    cleanup:
        sys::enable_interrupts();
        sys::panic("Storage manager init failed!");

    done:
        sys::enable_interrupts();
    }


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
    status_t 
    Storage_Manager::save_filesystem() noexcept {
        using namespace stdlib;
        using namespace filesys;

        status_t status;
        MOFS_Header header;
        static stdlib::Array<Stored_File_Entry, 
                             FILE_TABLE_ENTRYS> stored_file_entrys;
        uint32_t current_data_offset = 0;

        sys::disable_interrupts();

        stored_file_entrys.fill(Stored_File_Entry{});

        status = write_filesystem_header(header);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        for (uint32_t current_entry = 0; 
             current_entry < header.file_entry_count; 
             current_entry++) [[likely]] {
            File_Entry& 
            file_entry = MoleculeOS_File_System_2::get_file_entry(current_entry);

            if (file_entry.used_data_byte_size == 0) [[unlikely]] {
                continue;
            }

            status = store_single_file(current_data_offset,
                                       stored_file_entrys[current_entry],
                                       file_entry,
                                       header);
            if (status != status::SUCCESS) [[unlikely]] {
                goto cleanup;
            }
        }

        status = write_file_entry_table(stored_file_entrys.data(), 
                                        header);
        if (status != status::SUCCESS) [[unlikely]]{
            goto cleanup;
        }

        status = status::SUCCESS;

    cleanup:
        sys::enable_interrupts();

        return status;
    }


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
    status_t 
    Storage_Manager::load_filesystem() noexcept {
        using namespace drivers;
        
        status_t status;
        MOFS_Header header;
        static stdlib::Array<Stored_File_Entry, 
                             filesys::FILE_TABLE_ENTRYS> stored_file_entrys;
        uint32_t table_byte_size;

        sys::disable_interrupts();

        stored_file_entrys.fill(Stored_File_Entry{});

        status = read_filesystem_header(header);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        table_byte_size = header.file_entry_count * sizeof(Stored_File_Entry);

        status = read_or_write_bytes(stored_file_entrys.data(),
                                     table_byte_size,
                                     header.file_entry_table_offset,
                                     ata::Operations::READ);
        if (status != status::SUCCESS) [[unlikely]] {
            goto cleanup;
        }

        for (uint32_t current_entry = 0; 
            current_entry < header.file_entry_count; 
            current_entry++) [[likely]] {
            status = load_single_file(header, 
                                      stored_file_entrys[current_entry], 
                                      current_entry);
            if (status != status::SUCCESS) [[unlikely]] {
                goto cleanup;
            }
        }

        status = status::SUCCESS;

    cleanup:
        sys::enable_interrupts();

        return status;
    }
} // kernel::storemgr
