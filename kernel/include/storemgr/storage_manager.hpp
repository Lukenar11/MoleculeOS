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

#include <status.hpp>
#include <sal.hpp>


namespace kernel::storemgr
{
    /** 
     * @brief Initializes the storage manager. 
     * 
     * @note This function panics on failure. 
     */
    _API_ 
    void 
    init() noexcept;


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
    load_filesystem() noexcept;
} // namespace kernel::storemgr
