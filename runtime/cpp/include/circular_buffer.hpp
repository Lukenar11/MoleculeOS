#pragma once

#include <array.hpp>
#include <stdint.h>

namespace runtime
{
    template<typename T, uint32_t S>
    class Circular_Buffer final {
    private:
        runtime::Array<T, S> buffer;
        uint32_t buffer_head;
        uint32_t buffer_tail;
        bool buffer_is_full;

    public:
        bool push(const T& item, bool overwrite_last_item=false) noexcept {
            if (buffer_is_full) {
                if (!overwrite_last_item)
                    return false;

                buffer_tail = (buffer_tail + 1) % S;
            }

            buffer[buffer_head] = item;
            buffer_head = (buffer_head + 1) % S;
            buffer_is_full = (buffer_head == buffer_tail);
            return true;
        }

        bool pop(T& item) noexcept {
            if (empty())
                return false;

            item = buffer[buffer_tail];
            buffer_is_full = false;
            buffer_tail = (buffer_tail + 1) % S;
            return true;
        }

        bool peek(T& item) noexcept {
            if (empty())
                return false;

            item = buffer[buffer_tail];
            return true;
        }

        inline bool empty() const noexcept {
            return (!buffer_is_full && (buffer_head == buffer_tail));
        }

        inline bool is_full() const noexcept {
            return buffer_is_full;
        }

        inline constexpr uint32_t size() const noexcept {
            return S;
        }

        inline void reset() noexcept {
            buffer_head = buffer_tail;
            buffer_is_full = false;
        }

        inline constexpr Circular_Buffer() noexcept {
            buffer_head = 0;
            buffer_tail = 0;
            buffer_is_full = false;
        }

        ~Circular_Buffer() noexcept = default;
    };
} // namespace runtime
