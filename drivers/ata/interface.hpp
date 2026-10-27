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
         */
        _API_
        virtual
        status_t
        init() noexcept = 0;


        /**
         * @brief Main method performing read
         *        operations over a specific range.
         *
         * @param operation Operation struct with buffer pointer,
         *                  start address and range.
         */ 
        _API_
        [[nodiscard]]
        virtual 
        status_t
        load_sectors(_INOUT_ T& operation) noexcept = 0;


        /**
         * @brief Main method performing write
         *        operations over a specific range.
         *
         * @param operation Operation struct with buffer pointer,
         *                  start address and range.
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
