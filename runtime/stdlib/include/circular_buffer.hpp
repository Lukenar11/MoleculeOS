/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This header defines a minimal, fixed-size circular buffer template.

NOTES:
    The code is completely defined and declared in the header, 
    because of the typical C++ template chaos 😂 and 
    so that the compiler can inline certain methods.
*/


#pragma once

#include <types.hpp>
#include <array.hpp>
#include <sal.hpp>
#include <status.hpp>


namespace stdlib
{
    template<typename T, uint32_t S>
    class Circular_Buffer final {
    private:
        static_assert(S > 0, "'Circular_Buffer' size greater than zero");

        stdlib::Array<T, S> buffer;
        uint32_t buffer_head;
        uint32_t buffer_tail;
        bool buffer_is_full;


        /**
         * @brief Increments an index inside the circular buffer.
         * 
         * @warning No bounds checking is performed. 
         *          The caller must ensure that `i` is a valid buffer index.
         * 
         * @note This function performs modulo arithmetic to ensure that the
         *       index always stays within the valid range `[0...S-1]`.
         *
         * @param i The current index value.
         *
         * @return The incremented index, wrapped around the buffer size.
         */
        inline constexpr uint32_t 
        increment_index(_IN_ const uint32_t i) const noexcept {
            return (i + 1) % S;
        }


    public:
        /**
         * @brief Inserts an item into the circular buffer.
         *
         * @param item                The element to insert.
         * @param overwrite_last_item If `true`, the oldest element will be overwritten
         *                            when the buffer is full.
         *
         * @retval `status::FULL`
         *          The buffer is full and `overwrite_last_item` is `false`.
         *
         * @retval `status::SUCCESS | status::flags::OVERWRITTEN`
         *          The buffer was full, but the oldest item was overwritten.
         * 
         * @retval `status::SUCCESS`
         *          Default Case.
         */
        status_t 
        push(_IN_ const T& item, 
             _IN_ const bool overwrite_last_item=false) noexcept {
            status_t status;

            if (buffer_is_full) [[unlikely]] {
                if (!overwrite_last_item) [[unlikely]] {
                    status = status::FULL;
                    goto cleanup;
                }

                buffer_tail = increment_index(buffer_tail);
            }

            buffer[buffer_head] = item;
            buffer_head         = increment_index(buffer_head);
            buffer_is_full      = (buffer_head == buffer_tail);

            if (!overwrite_last_item) [[likely]] {
                status = status::SUCCESS;
            }
            else [[unlikely]] {
                status = status::SUCCESS | status::flags::OVERWRITTEN;
            }
        
        cleanup:
            return status;
        }


        /**
         * @brief Removes the oldest element from the buffer.
         *
         * @param item The removed element.
         * 
         * @retval `status::BUFFER_UNDERFLOW | status::EMPTY`
         *          If the buffer is empty.
         *
         * @retval `status::SUCCESS`
         *          Default case.
         */
        status_t 
        pop(_OUT_ T& item) noexcept {
            status_t status;

            if (empty()) [[unlikely]] {
                status = status::BUFFER_UNDERFLOW | status::flags::EMPTY;
                goto cleanup;
            }

            item           = buffer[buffer_tail];
            buffer_is_full = false;
            buffer_tail    = increment_index(buffer_tail);

            status = status::SUCCESS;
        
        cleanup:
            return status;
        }


        /**
         * @brief Reads the oldest element without removing it.
         *
         * @param item The element at the buffer tail.
         *
         * @retval `status::BUFFER_UNDERFLOW | status::EMPTY`
         *          If the buffer is empty.
         *
         * @retval `status::SUCCESS`
         *          Default case.
         */
        status_t 
        peek(_OUT_ T& item) const noexcept {
            status_t status;

            if (empty()) [[unlikely]] {
                status = status::BUFFER_UNDERFLOW | status::flags::EMPTY;
                goto cleanup;
            }

            item   = buffer[buffer_tail];
            status = status::SUCCESS;
        
        cleanup:
            return status;
        }

        
        /**
         * @brief Resets the buffer.
         */
        inline void 
        reset() noexcept {
            buffer_head    = 0;
            buffer_tail    = 0;
            buffer_is_full = false;
        }


        /**
         * @brief Returns the buffer size.
         *
         * @return The buffer capacity.
         */
        [[nodiscard]] inline constexpr uint32_t 
        capacity() const noexcept { 
            return S; 
        }


        /**
         * @brief Returns the number of elements in the buffer.
         * 
         * @note If the buffer is full, this function returns the buffer size.
         *
         * @return The number of stored elements.
         */
        [[nodiscard]] inline uint32_t 
        count() const noexcept {
            uint32_t count;

            if (buffer_is_full) [[unlikely]] {
                count = S;
                goto cleanup;
            }

            if (buffer_head >= buffer_tail) [[unlikely]] {
                count = buffer_head - buffer_tail;
                goto cleanup;
            }

            count = S + buffer_head - buffer_tail;

        cleanup:
            return count;
        }


        /**
         * @brief Checks whether the buffer is empty.
         *
         * @retval `true` If the buffer is empty.
         * @retval `false` Default case.
         */
        [[nodiscard]] inline constexpr bool 
        empty() const noexcept {
            return (!buffer_is_full && (buffer_head == buffer_tail));
        }


        /**
         * @brief Checks whether the buffer is full.
         *
         * @retval `true` If the buffer is full.
         * @retval `false` Default case.
         */
        [[nodiscard]] inline constexpr bool 
        is_full() const noexcept {
            return buffer_is_full;
        }


        /**
         * @brief Initializes an empty circular buffer.
         *
         * @note `buffer_head` & `buffer_tail` starts at `0` and
         *        buffer_is_full is `false`.
         */
        inline constexpr 
        Circular_Buffer() noexcept : 
            buffer_head(0), 
            buffer_tail(0), 
            buffer_is_full(false) {
        }


        ~Circular_Buffer() noexcept = default;
    };
} // namespace stdlib
