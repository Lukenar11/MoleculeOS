#pragma once

#include "Drivers/VGA/include/VGA.hpp"
#include <Atom/C/stdint.h>
#include <Atom/CPP/Array.hpp>

namespace terminal {

    enum class PrintFormat 
        : uint8_t {INTEGER, HEX, BINARY};

    class Terminal {

        private:
            // VGA-Driver
            static constexpr drivers::vga::VGA vga_driver;

            // default Cursor-Position & Color
            static constexpr uint8_t DEFAULT_CURSOR_POS = 0x00;
            static constexpr uint8_t DEFAULT_COLOR =
                drivers::vga::VGA::make_color(
                    drivers::vga::VGAColors::LIGHT_GREY,
                    drivers::vga::VGAColors::BLACK
                );
            
            int32_t cursor_x = DEFAULT_CURSOR_POS;
            int32_t cursor_y = DEFAULT_CURSOR_POS;
            uint8_t cursor_color = DEFAULT_COLOR;

        public:
            void reset() noexcept;
            void new_line() noexcept;

            void put_char(const char symbol) noexcept;
            void put_string(const char* message) noexcept;

            void print(int32_t value, const PrintFormat format) noexcept;

            Terminal() noexcept = default;
            ~Terminal() noexcept = default;
    };
} // namespace terminal
