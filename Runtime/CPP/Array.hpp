#pragma once

#include <C/stdint.h>

namespace runtime 
{
    template<class T, uint32_t S>
    class Array final {
        private:
            static_assert(S > 0, "Array size must be greater than zero");

            T data[S];

        public:
            // array index-access (arr[i])
            constexpr T& operator[](uint32_t index) noexcept { return data[index]; }
            constexpr const T& operator[](uint32_t index) const noexcept { return data[index]; }

            // _get_ array-size
            constexpr uint32_t size() const noexcept { return S; }

            // Iterators
            constexpr T* begin() noexcept { return data; }
            constexpr T* end() noexcept { return data + S; }

            constexpr const T* begin() const noexcept { return data; }
            constexpr const T* end() const noexcept { return data + S; }

            // _get_ first/last array-elememt
            constexpr T& front() noexcept { return data[0]; }
            constexpr T& back() noexcept { return data[S - 1]; }

            constexpr const T& front() const noexcept { return data[0]; }
            constexpr const T& back() const noexcept { return data[S - 1]; }

            // Repace array-data with "value"
            constexpr void fill(const T& value) noexcept {
                for (uint32_t i = 0; i < S; i++)
                    data[i] = value; 
            }

            Array() noexcept = default;
            ~Array() noexcept = default;

            Array(const Array&) = delete;
            Array& operator=(const Array&) = delete;
        };
} // namespace runtime
