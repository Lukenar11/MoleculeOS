/**
 * @brief Defines the interface for ATA driver properties and storage access.
 *
 * @copyright Copyright (c) 2026 Lukenar11 (Luke Matthes)
 * @license:  https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
 * @author    Lukenar11 (Luke Matthes)
 *
 * @note
 */


#pragma once

#include <types.hpp>
#include <status.hpp>
#include <sal.hpp>


namespace
{
    /**
     * @brief Provides an interface for driver properties.
     * 
     * @details Provides driver properties that are accessible at compile time
     *          and do not depend on a specific instance, such as the ATA
     *          protocol-specific configuration.
     */
    template<typename Func>
    class Static_ATA_Interface {
    public:
        /**
         * @brief Initializes the driver.
         * 
         * @note Is declared as `default`.
         */
        _API_
        Static_ATA_Interface() noexcept = default;


        /**
         * @brief Uninitializes the driver.
         * 
         * @note Is declared as `default`.
         */
        _API_
        ~Static_ATA_Interface() noexcept = default;


        /**
         * @brief Gets the size of one sector.
         * 
         * @note Can be called without any driver initialisation.
         * 
         * @retval sector byte size
         */
        _API_
        [[nodiscard]]
        static
        uint32_t
        sector_size() noexcept {
            return Func::sector_size();
        }
    };


    /**
     * @brief Provides an interface for storage access.
     * 
     * @details Provides runtime operations required by a hard drive driver,
     *          such as transferring data to and 
     *          from a specific memory address.
     */
    template <typename T>
    class Dynamic_ATA_Interface {
    public:
        /**
         * @brief Initializes the driver.
         * 
         * @note Is declared as `default`.
         */
        _API_
        Dynamic_ATA_Interface() noexcept = default;


        /**
         * @brief Uninitializes the driver.
         * 
         * @note Is declared as `default`.
         */
        _API_
        ~Dynamic_ATA_Interface() noexcept = default;


        /**
         * @brief Initializes the driver.
         * 
         * @brief Initializes the ATA-PIO driver.
         */
        _API_
        virtual
        status_t
        init() noexcept = 0;


        /**
         * @brief ATA-PIO main method performing chunked read
         *        operations over a specific sector range.
         *
         * @param operation Operation struct with buffer pointer,
         *                  start LBA and sector count.
         * 
         * @note If `operation.sector_count` is `0`, 
         *       the `operation.sector_count` isset to `256`.
         */ 
        _API_
        [[nodiscard]]
        virtual 
        status_t
        load_sectors(_INOUT_ T& operation) noexcept = 0;


        /**
         * @brief ATA-PIO main method performing chunked write
         *        operations over a specific sector range.
         *
         * @param operation Operation struct with buffer pointer,
         *                  start LBA and sector count.
         * 
         * @note If `operation.sector_count` is `0`, 
         *       the `operation.sector_count` isset to `256`.
         */ 
        _API_
        [[nodiscard]]
        virtual
        status_t
        store_sectors(_INOUT_ T& operation) noexcept = 0;
    };
} // namespace


namespace drivers::ata
{
    /**
     * @brief Links driver properties to operations for storage drivers.
     * 
     * @details It serves as a common foundation for 
     *          specific driver implementations.
     */
    template<typename Func, typename T>
    class Interface : public Static_ATA_Interface<Func>,
                      public Dynamic_ATA_Interface<T> {
    private:
        using Static_ATA_Interface<Func>::sector_size;


    public:
        /**
         * @brief Initializes the driver.
         * 
         * @note Is declared as `default`.
         */
        _API_
        Interface() noexcept = default;


        /**
         * @brief Uninitializes the driver.
         * 
         * @note Is declared as `default`.
         */
        _API_
        ~Interface() noexcept = default;
    };
} // namespace drivers::ata
