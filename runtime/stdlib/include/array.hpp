/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This header defines a minimal, fixed-size array template.

    Unlike 'std::array', this implementation avoids dependencies on the
    C++ standard library, which is unavailable in kernel mode.

NOTES:
*/


#pragma once

#include <types.hpp>
#include <sal.hpp>


namespace stdlib 
{
    template<class T, uint32_t S>
    class Array final {
    private:
        static_assert(S > 0, "'Array' size must be greater than zero");

        T buffer[S] = {};

    public:
        /**
         * @brief Returns the element at the given index.
         * 
         * @warning No bounds checking is performed.
         * 
         * @param index index of the element
         * 
         * @return reference to the element at the given index
         */
        _API_ inline constexpr T& 
        operator[](_IN_ uint32_t index) noexcept { 
            return buffer[index]; 
        }

        _API_ inline constexpr const T&
        operator[](_IN_ const uint32_t index) const noexcept {
            return buffer[index]; 
        }


        /**
         * @brief Returns the count of all array elements.
         */
        _API_ inline constexpr uint32_t 
        size() const noexcept { 
            return S; 
        }


        /**
         * @brief Returns a pointer to the first element.
         */
        _API_ inline constexpr T* 
        begin() noexcept { 
            return buffer; 
        }

        _API_ inline constexpr const T* 
        begin() const noexcept { 
            return buffer; 
        }


        /**
         * @brief Returns a pointer to the last element.
         */
        _API_ inline constexpr T* 
        end() noexcept { 
            return buffer + S; 
        }

        _API_ inline constexpr const T* 
        end() const noexcept { 
            return buffer + S; 
        }


        /**
         * @brief Returns the first element.
         *
         * @warning No bounds checking is performed.
         */
        _API_ inline constexpr T& 
        front() noexcept { 
            return buffer[0]; 
        }

        _API_ inline constexpr const T& 
        front() const noexcept { 
            return buffer[0];
        }


        /**
         * @brief Returns the last element.
         *
         * @warning No bounds checking is performed.
         */
        _API_ inline constexpr T& 
        back() noexcept { 
            return buffer[S - 1]; 
        }

        _API_ inline constexpr const T& 
        back() const noexcept { 
            return buffer[S - 1]; 
        }


        /**
         * @brief Returns a pointer to the data buffer.
         */
        _API_ inline constexpr T* 
        data() noexcept { 
            return buffer; 
        }

        _API_ inline constexpr const T* 
        data() const noexcept { 
            return buffer; 
        }


        /**
         * @brief Fills all array elements with a specific value.
         * 
         * @param value Value with which the array should be filled.
         * 
         */
        _API_ inline constexpr void 
        fill(_IN_ const T& value) noexcept {
            for (uint32_t i = 0; i < S; i++) [[likely]] {
                buffer[i] = value; 
            }
        }

        Array() noexcept  = default;
        ~Array() noexcept = default;
    };
} // namespace stdlib
