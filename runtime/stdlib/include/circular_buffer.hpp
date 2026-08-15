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

        inline constexpr
        uint32_t increment_index(_IN_ const uint32_t i) const noexcept {
            return (i + 1) % S;
        }

    public:
        status_t push(_IN_ const T& item, 
                      _IN_ bool overwrite_last_item=false) noexcept {
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

            status = status::SUCCESS;
        
        cleanup:
            return status;
        }

        status_t pop(_OUT_ T& item) noexcept {
            status_t status;

            if (empty()) [[unlikely]] {
                status = status::EMPTY;
                goto cleanup;
            }

            item           = buffer[buffer_tail];
            buffer_is_full = false;
            buffer_tail    = increment_index(buffer_tail);

            status = status::SUCCESS;
        
        cleanup:
            return status;
        }

        status_t peek(_OUT_ T& item) const noexcept {
            status_t status;

            if (empty()) [[unlikely]] {
                status = status::EMPTY;
                goto cleanup;
            }

            item   = buffer[buffer_tail];
            status = status::SUCCESS;
        
        cleanup:
            return status;
        }
        
        inline void reset() noexcept {
            buffer_head    = 0;
            buffer_tail    = 0;
            buffer_is_full = false;
        }

        [[nodiscard]] inline constexpr 
        uint32_t capacity() const noexcept { 
            return S; 
        }

        [[nodiscard]] inline 
        uint32_t count() const noexcept {
            uint32_t status;

            if (buffer_is_full) [[unlikely]] {
                status = S;
                goto cleanup;
            }

            if (buffer_head >= buffer_tail) [[unlikely]] {
                status = buffer_head - buffer_tail;
                goto cleanup;
            }

            status = S + buffer_head - buffer_tail;

        cleanup:
            return status;
        }

        [[nodiscard]] inline constexpr
        bool empty() const noexcept {
            return (!buffer_is_full && (buffer_head == buffer_tail));
        }

        [[nodiscard]] inline constexpr
        bool is_full() const noexcept {
            return buffer_is_full;
        }

        inline constexpr Circular_Buffer() noexcept
            : buffer_head(0), buffer_tail(0), buffer_is_full(false) {}

        ~Circular_Buffer() noexcept = default;
    };
} // namespace stdlib
