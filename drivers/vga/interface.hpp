#pragma once

#include <types.hpp>
#include <sal.hpp>
#include <status.hpp>


namespace
{
    template<typename Func>
    class Static_Interface {
    public:
        /**
         * @brief Initializes the driver.
         */
        _API_
        Static_Interface() noexcept = default;


        /**
         * @brief Uninitializes the driver
         * 
         * @note It's declared as default.
         */
        _API_
        ~Static_Interface() noexcept = default;


        /**
         * @brief Gets the screen width.
         * 
         * @note It's `static` and 
         *       can be called without any driver initialisation.
         * 
         * @return width of the screen
         */
        _API_
        [[nodiscard]] 
        static 
        constexpr 
        uint32_t 
        screen_width() noexcept {
            return Func::screen_width();
        }


        /**
         * @brief Gets the screen height.
         * 
         * @note It's `static` and 
         *       can be called without any driver initialisation.
         * 
         * @return height of the screen
         */
        _API_
        [[nodiscard]] 
        static 
        constexpr 
        uint32_t 
        screen_height() noexcept {
            return Func::screen_height();
        }
    };


    template<typename T>
    class Dynamic_Interface {
    public:
        /**
         * @brief Initializes the driver.
         */
        _API_
        Dynamic_Interface() noexcept = default;


        /**
         * @brief Uninitializes the driver
         * 
         * @note It's declared as default.
         */
        _API_
        ~Dynamic_Interface() noexcept = default;


        /** 
         * @brief Clears the screen and sets a special screen color.
         *
         * @param screen_color screen color
         */
        _API_ 
        virtual
        void 
        clear_screen(_IN_ const T& screen_color) noexcept = 0;


        /**
         * @brief Outputs a character with a special fore and background color.
         * 
         * @param x      X coordinate for the output
         * @param y      Y coordinate for the output
         * @param symbol character output with the colors
         */
        _API_ 
        [[nodiscard]]
        virtual
        status_t 
        set_symbol_at(_IN_ const uint32_t x,
                      _IN_ const uint32_t y,
                      _IN_ const T& symbol) noexcept = 0;


        /**
         * @brief Get the char and his color that was output at the 
         *        entered coordinate.
         * 
         * @param symbol outputted char with the color
         * @param x      x coordinate of the outputted symbol
         * @param y      y coordinate of the outputted symbol
         */
        _API_
        [[nodiscard]]
        virtual
        status_t
        get_symbol_at(_OUT_ T& symbol,
                      _IN_  const uint32_t x,
                      _IN_  const uint32_t y) noexcept = 0;
    };
} // namespace


namespace drivers::vga
{
    template<typename Func, typename T>
    class Interface : public Static_Interface<Func>, public Dynamic_Interface<T> {
    private:
        using Static_Interface<Func>::screen_width;
        using Static_Interface<Func>::screen_height;


    public:
        /**
         * @brief Initializes the driver.
         * 
         * @note It's declared as default.
         */
        _API_
        Interface() noexcept = default;


        /**
         * @brief Uninitializes the driver
         * 
         * @note It's declared as default.
         */
        _API_
        ~Interface() noexcept = default;
    };
} // namespace drivers::vga
