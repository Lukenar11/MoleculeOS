#pragma once

#include <types.hpp>
#include <sal.hpp>
#include <status.hpp>


namespace
{
    template<typename Static_Derived>
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
            return Static_Derived::screen_width();
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
            return Static_Derived::screen_height();
        }
    };


    template<typename Derived_Color>
    class Dynamic_Interface {
    public:
        using color_t = Derived_Color;


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
         * @brief Makes a specific symbol color layout for the screen buffer.
         * 
         * @param foreground symbol color foreground
         * @param background symbol color background
         * @param blink      symbol color blink mode (default case `false`)
         * 
         * @return symbol color layout
         */
        _API_ 
        [[nodiscard]] 
        virtual
        uint8_t
        make_symbol_color(_IN_ const color_t foreground,
                          _IN_ const color_t background,
                          _IN_ const bool blink=false) noexcept = 0;


        /** 
         * @brief Clears the screen and sets a special screen color.
         * 
         * @param foreground screen text color (default case `BLACK`)
         * @param background screen color (default case `BLACK`)
         */
        _API_ 
        virtual
        void 
        clear_screen(_IN_ const color_t foreground,
                     _IN_ const color_t background) noexcept = 0;


        /**
         * @brief Outputs a character with a special fore and background color.
         * 
         * @note The color must match a specific layout, 
         *       use `Text_Mode::make_symbol_color` 
         *       to ensure the color layout is correct.
         * 
         * @param x      X coordinate for the output
         * @param y      Y coordinate for the output
         * @param color  character color
         * @param symbol character output
         */
        _API_ 
        [[nodiscard]] 
        virtual
        status_t 
        set_symbol_at(_IN_ const uint32_t x,
                      _IN_ const uint32_t y,
                      _IN_ const uint8_t color,
                      _IN_ const char symbol) noexcept = 0;


        /**
         * @brief Get the char and his color that was output at the 
         *        entered coordinate.
         * 
         * @param symbol outputted char
         * @param color  color of the outputted char
         * @param x      x coordinate of the outputted symbol
         * @param y      y coordinate of the outputted symbol
         *          
         * @retval `status::SUCCESS`
         */
        _API_
        [[nodiscard]] 
        virtual
        status_t
        get_symbol_at(_OUT_ char& symbol,
                      _OUT_ uint8_t& color,
                      _IN_  const uint32_t x,
                      _IN_  const uint32_t y) noexcept = 0;
    };
} // namespace


namespace drivers::vga
{
    template<typename Static_Derived, typename Derived_Color>
    class Interface : 
        public Static_Interface<Static_Derived>, 
        public Dynamic_Interface<Derived_Color> {
    public:
        using color_t = Derived_Color;

        using Static_Interface<Static_Derived>::screen_width;
        using Static_Interface<Static_Derived>::screen_height;


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
