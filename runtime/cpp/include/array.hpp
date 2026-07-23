/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This header defines a minimal, fixed-size array template.

    Unlike "'std::array', this implementation avoids dependencies on the
    C++ standard library, which is unavailable in kernel mode.

NOTES:
*/

#pragma once

#include <stdint.h>

namespace runtime 
{
    template<class T, uint32_t S>
    class Array final {
    private:
        static_assert(S > 0, "Array size must be greater than zero");

        T buffer[S] = {};

    public:
        // array index-access (arr[i])
        inline constexpr T& operator[](uint32_t index) noexcept { 
            return buffer[index]; 
        }

        inline constexpr const T& operator[](const uint32_t index) 
                                             const noexcept {
            return buffer[index]; 
        }

        // get array-size
        inline constexpr uint32_t size() const noexcept { return S; }

        // iterators
        inline constexpr T* begin() noexcept { return buffer; }
        inline constexpr T* end() noexcept { return buffer + S; }

        inline constexpr const T* begin() const noexcept { return buffer; }
        inline constexpr const T* end() const noexcept { return buffer + S; }

        // get first/last array-element
        inline constexpr T& front() noexcept { return buffer[0]; }
        inline constexpr T& back() noexcept { return buffer[S - 1]; }

        inline constexpr const T& front() const noexcept { return buffer[0]; }
        inline constexpr const T& back() const noexcept { return buffer[S - 1]; }

        // get array-content
        inline constexpr T* data() noexcept { return buffer; }
        inline constexpr const T* data() const noexcept { return buffer; }

        // Replace array-data with "value"
        inline constexpr void fill(const T& value) noexcept {
            for (uint32_t i = 0; i < S; i++)
                buffer[i] = value; 
        }

        Array() noexcept  = default;
        ~Array() noexcept = default;
    };
} // namespace runtime
