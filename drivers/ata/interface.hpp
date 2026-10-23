#pragma once

#include <types.hpp>
#include <status.hpp>
#include <sal.hpp>


namespace drivers::ata
{
    class Interface {
    public:
        /**
         * @brief Uninitializes the driver.
         * 
         * @note Is declared as default.
         */
        Interface() noexcept = default;


        /**
         * @brief Uninitializes the driver.
         * 
         * @note Is declared as default.
         */
        ~Interface() noexcept = default;


        /**
         * @brief Initializes the driver.
         */
        _API_
        virtual
        void
        init() noexcept;


        /**
         * @brief Interface performing chunked write
         *        operations over a specific sector range.
         *
         * @param buffer       I/O buffer for sector data
         * @param relative_lba Starting LBA address
         * @param sector_count Number of sectors to transfer
         */ 
        _API_
        [[nodiscard]]
        virtual 
        status_t
        load_sectors(_INOUT_ uint16_t* buffer,
                     _IN_    uint32_t relative_lba,
                     _IN_    uint32_t sector_count) noexcept;


        /**
         * @brief Interface performing chunked read
         *        operations over a specific sector range.
         *
         * @param buffer       I/O buffer for sector data
         * @param relative_lba Starting LBA address
         * @param sector_count Number of sectors to transfer
         */ 
        _API_
        [[nodiscard]]
        virtual
        status_t
        store_sectors(_INOUT_ uint16_t* buffer,
                      _IN_    uint32_t relative_lba,
                      _IN_    uint32_t sector_count) noexcept;


        /**
         * @brief Gets the sector size of one sector.
         * 
         * @retval sector size
         */
        _API_
        [[nodiscard]]
        virtual
        uint32_t
        sector_size() const noexcept;
    };
} // namespace drivers::ata
