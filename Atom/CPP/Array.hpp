#pragma once

#include <stdint.h>
#include <C/string.h>

namespace atom {

    template<class T, uint32_t S>
    class Array final {

        private:
            static_assert(S > 0, "Array size must be greater than zero");
            T data[S];
        
        public:
            // Data access
            inline constexpr T& operator[]
                (uint32_t index) noexcept {return data[index];}

            inline constexpr const T& operator[]
                (uint32_t index) const noexcept {return data[index];}

            // _get_ size
            inline constexpr uint32_t size() const noexcept {return S;}

            // Iterators
            inline constexpr T* begin() noexcept {return data;}
            inline constexpr T* end() noexcept {return data + S;}

            inline constexpr const T* begin() const noexcept {return data;}
            inline constexpr const T* end() const noexcept {return data + S;}

            // Element access
            inline constexpr T& front() noexcept {return data[0];}
            inline constexpr T& back() noexcept {return data[S - 1];}

            inline constexpr const T& front() const noexcept {return data[0];}
            inline constexpr const T& back() const noexcept {return data[S - 1];}

            // fill with value
            constexpr void fill(const T& value) noexcept {

                for (uint32_t i = 0; i < S; ++i)
                    data[i] = value;
            }
    
            // _construct_ (for "atom::Array<...> name(...);")
            template<class... Args>
            constexpr Array(Args... args) noexcept : data {static_cast<T>(args)...} {
    
                static_assert(
                    sizeof...(Args) == S,
                    "Number of constructor arguments must match array size"
                );
            }

            Array(const Array&) = delete;
            Array& operator=(const Array&) = delete;

            constexpr Array() noexcept = default;
            ~Array() noexcept = default;
    };
} // namespace atom
