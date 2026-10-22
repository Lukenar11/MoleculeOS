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

#include <drivers.hpp>
#include <kernel.hpp>
#include <memory.hpp>
#include <string.hpp>


namespace
{
    using namespace kernel;
    using namespace storemgr;


    struct MOFS_Header final {
        stdlib::Array<char, 5> magic;
        uint32_t version                 = 0;
        uint32_t file_entry_count        = 0;
        uint32_t file_entry_table_offset = 0;
        uint32_t data_offset             = 0;
    };

    struct Stored_File_Entry final {
        stdlib::Array<char, filesys::MAX_FILE_NAME_LENGTH + 1> file_name;
        stdlib::Array<char, filesys::MAX_FILE_FORMAT_LENGTH + 1> file_format;
        uint32_t name_hash           = 0;
        uint32_t format_hash         = 0;
        uint32_t file_byte_size      = 0;
        uint32_t used_data_byte_size = 0;
        uint32_t file_data_offset    = 0;
    };


    constexpr const char* MOFS_HEADER_MAGIC = "MOFS";

    constexpr uint32_t MOFS_VERSION            = 2;
    constexpr uint32_t MOFS_HEADER_OFFSET      = 0;
    constexpr uint32_t SECTOR_SIZE             = drivers::ata::SECTOR_SIZE;
    constexpr uint32_t FILE_ENTRY_TABLE_OFFSET = SECTOR_SIZE;
    constexpr uint32_t FILESYS_DATA_OFFSET     = FILE_ENTRY_TABLE_OFFSET + 
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
     *          If `buffer_ptr` is `nullptr`.
     *
     * @retval `status::NULL_POINTER | status::flags::PARAM_B`
     *          If `temp_buffer_ptr` is `nullptr`.
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
    validate_byte_reads_and_writes(_IN_ const uint16_t* const buffer_ptr,
                                   _IN_ const uint16_t* const temp_buffer_ptr,
                                   _IN_ const uint32_t byte_size,
                                   _IN_ const uint32_t sector_count) noexcept {
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
     *          If `buffer_ptr` is `nullptr`. 
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_B` 
     *          If `temp_buffer_ptr` is `nullptr`. 
     * 
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_C` 
     *          If `byte_size` is `0`. 
     * 
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_D` 
     *          If `sector_count` is `0`. 
     *
     * @retval `status::IO_ERROR`
     *          If the hardware reports an error.
     * 
     * @retval `status::FAIL`
     *          Unknown error.
     * 
     * @retval `status::SUCCESS` 
     *          Default case. 
     */
    status_t 
    read_bytes(_OUT_ uint16_t* const buffer_ptr,
               _IN_  uint16_t* const temp_buffer_ptr,
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
            if (status & status::flags::PARAM_A) {
                status ^= status::flags::PARAM_A;
                status |= status::flags::PARAM_B;
            }
            else if (status == status::ATA_ERROR || 
                     status == status::ATA_TIMEOUT || 
                     status == status::ATA_DEVICE_FAULT) {
                status = status::IO_ERROR;
            }
            else {
                status = status::FAIL;
            }

            goto cleanup;
        }

        dest_ptr = reinterpret_cast<uint8_t*>(temp_buffer_ptr) + sector_offset;

        status = stdlib::memory::copy_block(buffer_ptr, dest_ptr, byte_size);
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
     *          If `buffer_ptr` is `nullptr`. 
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_B` 
     *          If `temp_buffer_ptr` is `nullptr`. 
     * 
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_C` 
     *          If `byte_size` is `0`. 
     * 
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_D` 
     *          If `sector_count` is `0`. 
     * 
     * @retval `status::IO_ERROR`
     *          If the hardware reports an error.
     * 
     * @retval `status::FAIL`
     *          Unknown error.
     * 
     * @retval `status::SUCCESS` 
     *          Default case. 
     */
    status_t 
    write_bytes(_IN_ uint16_t* const buffer_ptr,
                _IN_ uint16_t* const temp_buffer_ptr,
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
            if (status & status::flags::PARAM_A) {
                status ^= status::flags::PARAM_A;
                status |= status::flags::PARAM_B;
            }
            else if (status == status::ATA_ERROR || 
                     status == status::ATA_TIMEOUT || 
                     status == status::ATA_DEVICE_FAULT) {
                status = status::IO_ERROR;
            }
            else {
                status = status::FAIL;
            }

            goto cleanup;
        }
    
        dest_ptr = reinterpret_cast<uint8_t*>(temp_buffer_ptr) + sector_offset;

        status = stdlib::memory::copy_block(dest_ptr, buffer_ptr, byte_size);
        if (status != status::SUCCESS) [[unlikely]] {
            if (status & status::flags::PARAM_A) {
                status ^= status::flags::PARAM_A;
                status |= status::flags::PARAM_B;
            }
            else {
                status ^= status::flags::PARAM_B;
                status |= status::flags::PARAM_A;
            }

            goto cleanup;
        }
    
        status = ata::Programmable_Input_Output::run(temp_buffer_ptr,
                                                     start_sector,
                                                     sector_count,
                                                     ata::Operations::WRITE);
        if (status != status::SUCCESS) [[unlikely]] {
            if (status & status::flags::PARAM_A) {
                status ^= status::flags::PARAM_A;
                status |= status::flags::PARAM_B;
            }
            else if (status == status::ATA_ERROR || 
                     status == status::ATA_TIMEOUT || 
                     status == status::ATA_DEVICE_FAULT) {
                status = status::IO_ERROR;
            }
            else {
                status = status::FAIL;
            }

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
     *          If `buffer_ptr` is `nullptr`. 
     * 
     * @retval `status::INVALID_PARAMETER | status::flags::PARAM_B`
     *          If `byte_size` is `0`. 
     * 
     * @retval `status::BUFFER_OVERFLOW`
     *          If `byte_size` is to big.
     * 
     * @retval `status::FAIL`
     *          Unknown error.
     * 
     * @retval `status::IO_ERROR`
     *          If the hardware reports an error.
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     */
    status_t
    read_or_write_bytes(_INOUT_ void* const buffer_ptr,
                        _IN_    const uint32_t byte_size,
                        _IN_    const uint32_t offset,
                        _IN_    const drivers::ata::Operations operation) noexcept {
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
            status = status::FAIL;
            goto cleanup;
        }

        allocation_size = sector_count * ata::SECTOR_SIZE;

        if (allocation_size / ata::SECTOR_SIZE != sector_count) [[unlikely]] {
            status = status::BUFFER_OVERFLOW;
            goto cleanup;
        }

        status = heap::allocate(ptr, allocation_size);
        if (status != status::SUCCESS) [[unlikely]] {
            status = status::FAIL;

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

        if (status != status::SUCCESS &&
            status != status::IO_ERROR && 
            status != status::FAIL &&
            !(status & status::flags::PARAM_A)) [[unlikely]] {
            if (status & status::flags::PARAM_C) {
                status ^= status::flags::PARAM_C;
                status |= status::flags::PARAM_B;
            }
            else {
                status = status::FAIL;
            }

            goto cleanup;
        }

    cleanup:
        if (ptr) {
            if (heap::deallocate(ptr) 
                != status::SUCCESS) [[unlikely]] {
                status = status::FAIL;
            }
        }

        return status;
    }


    /** 
     * @brief Reads the filesystem header from the 
     *        first sector of the hard drive.
     * 
     * @param header Header of the filesystem.
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_A` 
     *          If `header` is `nullptr`. 
     * 
     * @retval `status::FS_VERSION_MISMATCH` 
     *          If the filesystem version is invalid. 
     * 
     * @retval `status::FS_INVALID_FILE_ENTRY` 
     *          If the magic value is invalid. 
     * 
     * @retval `status::IO_ERROR`
     *          If the hardware reports an error.

     * @retval `status::FAIL`
     *          Unknown error.
     * 
     * @retval `status::SUCCESS` 
     *          Default case. 
     */
    status_t 
    read_filesystem_header(_OUT_ MOFS_Header& header) noexcept {
        using namespace drivers;
        using namespace stdlib;

        status_t status;

        status = read_or_write_bytes(&header,
                                     sizeof(MOFS_Header),
                                     MOFS_HEADER_OFFSET,
                                     ata::Operations::READ);
        if (status != status::SUCCESS) [[unlikely]] {
            if (status != (status::NULL_POINTER | status::flags::PARAM_A) &&
                status != status::FAIL &&
                status != status::IO_ERROR) {
                status = status::FAIL;
            }

            goto cleanup;
        }

        if (header.version != MOFS_VERSION) [[unlikely]] {
            status = status::FS_VERSION_MISMATCH;
            goto cleanup;
        }

        status = stdlib::string::compare_strings(header.magic.data(), MOFS_HEADER_MAGIC);
        if (status != status::EQUAL_TO) [[unlikely]] {
            if (status & status::flags::PARAM_A) {
                goto cleanup;
            }

            status = status::FS_INVALID_FILE_ENTRY;
            goto cleanup;
        }

        status = status::SUCCESS;

    cleanup:
        return status;
    }


    /** 
     * @brief Stores the filesystem header from the RAM to the 
     *        first sector of the hard drive.
     * 
     * @param header Header of the filesystem.
     * 
     * @retval `status::NULL_POINTER | status::flags::PARAM_A` 
     *          If `header` is `nullptr`. 
     * 
     * @retval `status::IO_ERROR`
     *          If the hardware reports an error.
     * 
     * @retval `status::FS_CORRUPTED`
     *          If the filesystem header magic isn't correct.
     *
     * @retval `status::FAIL`
     *          Unknown error.
     * 
     * @retval `status::SUCCESS` 
     *          Default case. 
     */
    status_t 
    write_filesystem_header(_IN_ MOFS_Header& header) noexcept {
        using namespace filesys;
        using namespace drivers;

        status_t status;

        status = stdlib::string::copy_string(header.magic.data(), MOFS_HEADER_MAGIC);
        if (status != status::SUCCESS) [[unlikely]] {
            if (status & status::flags::PARAM_B) {
                status = status::FAIL;
            }
            else if (!(status & status::flags::PARAM_A)) {
                status = status::FS_CORRUPTED;
            }

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
            if (status != (status::NULL_POINTER | status::flags::PARAM_A) &&
                status != status::FAIL &&
                status != status::IO_ERROR) {
                status = status::FAIL;
            }

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
     * @retval `status::IO_ERROR`
     *          If the hardware reports an error.
     * 
     * @retval `status::FAIL`
     *          Unknown error.
     * 
     * @retval `status::SUCCESS` 
     *          Default case. 
     */
    status_t 
    load_single_file(_IN_ const MOFS_Header& header,
                     _IN_ const Stored_File_Entry& stored_file_entry,
                     _IN_ const uint32_t file_entry_index) noexcept {
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

        status = heap::allocate(data_ptr, 
                                                 stored_file_entry.file_byte_size);
        if (status != status::SUCCESS) [[unlikely]] {
            if (status == status::HEAP_EXHAUSTED) {
                status = status::FAIL;
            }

            goto done;
        }

        file_offset = header.data_offset + stored_file_entry.file_data_offset;

        status = read_or_write_bytes(data_ptr,
                                     stored_file_entry.file_byte_size,
                                     file_offset,
                                     ata::Operations::READ);
        if (status != status::SUCCESS) [[unlikely]] {
            if (status & status::flags::PARAM_A) {
                status = status::FAIL;
            }

            goto cleanup;
        }

        status = stdlib::memory::copy_block(file_entry.file_name.data(),
                                            stored_file_entry.file_name.data(),
                                            name_length);
        if (status != status::SUCCESS) [[unlikely]] {
            if (status & status::flags::PARAM_A) {
                status = status::FAIL;
            }

            goto cleanup;
        }

        status = stdlib::memory::copy_block(file_entry.file_format.data(),
                                            stored_file_entry.file_format.data(),
                                            format_length);
        if (status != status::SUCCESS) [[unlikely]] {
            if (status & status::flags::PARAM_A) {
                status = status::FAIL;
            }

            goto cleanup;
        }

        file_entry.name_hash           = stored_file_entry.name_hash;
        file_entry.format_hash         = stored_file_entry.format_hash;
        file_entry.file_byte_size      = stored_file_entry.file_byte_size;
        file_entry.used_data_byte_size = stored_file_entry.used_data_byte_size;
        file_entry.file_data_ptr       = data_ptr;

        if (file_entry_index >= filesys::FILE_TABLE_ENTRYS) [[unlikely]] {
            status = status::FAIL;
            goto cleanup;
        }

        filesys::set_file_entry(file_entry, file_entry_index);

        status = status::SUCCESS;

        goto done;

    cleanup:
        if (heap::deallocate(data_ptr) 
            != status::SUCCESS) [[unlikely]] {
            status = status::FAIL;
        }

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
     * @retval `status::FAIL` 
     *          Unknown error.
     * 
     * @retval `status::IO_ERROR` 
     *          If the hardware reports an error. 
     * 
     * @retval `status::SUCCESS` 
     *          Default case. 
     */
    status_t 
    store_single_file(_INOUT_ uint32_t& current_data_offset,
                      _OUT_   Stored_File_Entry& stored_file_entry,
                      _IN_    const filesys::File_Entry& file_entry,
                      _IN_    const MOFS_Header& header) noexcept {
        using namespace filesys;
        using namespace drivers;
        using namespace stdlib;

        status_t status;

        if (!file_entry.file_data_ptr) [[unlikely]] {
            status = status::SUCCESS;
            goto cleanup;
        }

        status = stdlib::memory::copy_block(stored_file_entry.file_name.data(),
                                            file_entry.file_name.data(),
                                            MAX_FILE_NAME_LENGTH + 1);
        if (status != status::SUCCESS) [[unlikely]] {
            if (status & status::flags::PARAM_A) {
                status ^= status::flags::PARAM_A;
                status |= status::flags::PARAM_B;
            }
            else if (status & status::flags::PARAM_B) {
                status ^= status::flags::PARAM_B;
                status |= status::flags::PARAM_C;
            }

            goto cleanup;
        }

        status = stdlib::memory::copy_block(stored_file_entry.file_format.data(),
                                            file_entry.file_format.data(),
                                            MAX_FILE_FORMAT_LENGTH + 1);
        if (status != status::SUCCESS) [[unlikely]] {
            if (status & status::flags::PARAM_A) {
                status ^= status::flags::PARAM_A;
                status |= status::flags::PARAM_B;
            }
            else if (status & status::flags::PARAM_B) {
                status ^= status::flags::PARAM_B;
                status |= status::flags::PARAM_C;
            }

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
            if (status & status::flags::PARAM_A ||
                status & status::flags::PARAM_B) {
                status ^= status::flags::PARAM_A;
                status ^= status::flags::PARAM_B;
                status |= status::flags::PARAM_C;
            }

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
     *          If `entry_table_ptr` is `nullptr`. 
     * 
     * @retval `status::IO_ERROR`
     *          If the hardware reports an error.
     * 
     * @retval `status::BUFFER_OVERFLOW`
     *          If the file entry count in `header` is wrong.
     * 
     * @retval `status::FAIL`
     *          Unknown error.
     * 
     * @retval `status::SUCCESS` 
     *          Default case. 
     */ 
    status_t 
    write_file_entry_table(_OUT_ Stored_File_Entry* const entry_table_ptr,
                           _IN_  const MOFS_Header& header) noexcept {
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
            if (!(status & status::BUFFER_OVERFLOW) && 
                !(status & status::IO_ERROR) &&
                !(status & status::flags::PARAM_B)) {
                status = status::FAIL;
            }

            goto cleanup;
        }

        status = status::SUCCESS;

    cleanup:
        return status;
    }
} // namespace


namespace kernel::storemgr
{
    /** 
     * @brief Initializes the storage manager. 
     * 
     * @note This function panics on failure. 
     */
    _API_
    void
    init() noexcept {
        using namespace drivers;
        using namespace filesys;
        using namespace stdlib;
    
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
    
        status = stdlib::string::compare_strings(header.magic.data(), MOFS_HEADER_MAGIC);
        if (status != status::EQUAL_TO) [[unlikely]] {
            status = stdlib::string::copy_string(header.magic.data(), MOFS_HEADER_MAGIC);
            if (status != status::SUCCESS) [[unlikely]] {
                goto cleanup;
            }

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
    
        status = status::SUCCESS;

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
     * @retval `status::IO_ERROR`
     *          If the hardware reports an error.
     * 
     * @retval `status::FS_CORRUPTED`
     *          If the filesystem header magic isn't correct.
     * 
     * @retval `status::BUFFER_OVERFLOW` 
     *          If the file entry count in filesystem header is wrong.
     * 
     * @retval `status::FAIL`
     *          Unknown error.
     * 
     * @retval `status::SUCCESS`
     *          Default case.
     */
    _API_ 
    status_t 
    save_filesystem() noexcept {
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
            if (status & status::flags::PARAM_A) {
                status = status::FAIL;
            }

            goto cleanup;
        }

        if (header.file_entry_count != FILE_TABLE_ENTRYS) [[unlikely]] {
            status = status::BUFFER_OVERFLOW;
            goto cleanup;
        }

        for (uint32_t current_entry = 0; 
             current_entry < header.file_entry_count; 
             current_entry++) [[likely]] {
            File_Entry& 
            file_entry = get_file_entry(current_entry);

            status = store_single_file(current_data_offset,
                                       stored_file_entrys[current_entry],
                                       file_entry,
                                       header);
            if (status != status::SUCCESS) [[unlikely]] {
                if (status & status::flags::PARAM_A ||
                    status & status::flags::PARAM_B ||
                    status & status::flags::PARAM_C) {
                    status = status::FAIL;
                }

                goto cleanup;
            }
        }

        status = write_file_entry_table(stored_file_entrys.data(), 
                                        header);
        if (status != status::SUCCESS) [[unlikely]] {
            if (status & status::flags::PARAM_A) {
                status = status::FAIL;
            }

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
     * @retval `status::IO_ERROR`
     *          If the hardware reports an error.
     * 
     * @retval `status::BUFFER_OVERFLOW` 
     *          If the file entry count in filesystem header is wrong.
     *
     * @retval `status::FAIL`
     *          Unknown error.
     * 
     * @retval `status::SUCCESS` 
     *          Default case. 
     */
    _API_ 
    status_t 
    load_filesystem() noexcept {
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
            if (status & status::flags::PARAM_A) {
                status = status::FAIL;
            }

            goto cleanup;
        }

        if (header.file_entry_count 
            != filesys::FILE_TABLE_ENTRYS) [[unlikely]] {
            status = status::BUFFER_OVERFLOW;
            goto cleanup;
        }

        table_byte_size = header.file_entry_count * sizeof(Stored_File_Entry);

        status = read_or_write_bytes(stored_file_entrys.data(),
                                     table_byte_size,
                                     header.file_entry_table_offset,
                                     ata::Operations::READ);
        if (status != status::SUCCESS) [[unlikely]] {
            if (status & status::flags::PARAM_A || 
                status & status::flags::PARAM_B) {
                status = status::FAIL;
            }

            goto cleanup;
        }

        for (uint32_t current_entry = 0; 
            current_entry < header.file_entry_count; 
            current_entry++) [[likely]] {
            status = load_single_file(header, 
                                      stored_file_entrys[current_entry], 
                                      current_entry);
            if (status != status::SUCCESS) [[unlikely]] {
                if (status & status::flags::PARAM_B) {
                    status = status::FAIL;
                }

                goto cleanup;
            }
        }

        status = status::SUCCESS;

    cleanup:
        sys::enable_interrupts();

        return status;
    }
} // namespace kernel::storemgr
