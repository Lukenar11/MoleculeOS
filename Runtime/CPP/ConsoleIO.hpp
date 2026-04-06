#pragma once

#include "Drivers/VGA/include/VGA.hpp"
#include <C/stdint.h>
#include <stdarg.h>
#include <Runtime/CPP/Array.hpp>

namespace runtime {

    class ConsoleIO {

        private:
            // VGA-Driver
            static constexpr drivers::vga::VGA vga_driver;

            // default Cursor-Position & Color
            static constexpr uint8_t DEFAULT_CURSOR_POS = 0x00;
            static constexpr uint8_t DEFAULT_COLOR =
                vga_driver.make_color(
                    drivers::vga::VGAColors::LIGHT_GREY,
                    drivers::vga::VGAColors::BLACK
                );
            
            int32_t cursor_x = DEFAULT_CURSOR_POS;
            int32_t cursor_y = DEFAULT_CURSOR_POS;
            uint8_t cursor_color = DEFAULT_COLOR;

            void new_line() noexcept;

        public:
            void reset() noexcept;
            void set_char_colors(
                const drivers::vga::VGAColors color, 
                const drivers::vga::VGAColors background) noexcept;

            void put_char(const char symbol) noexcept;
            void put_string(const char* message) noexcept;
            void put_int(int32_t value) noexcept;
            void put_uint(uint32_t value) noexcept;
            void put_hex(uint32_t value) noexcept;
            void put_bin(uint32_t value) noexcept;
            void put_ptr(uint32_t value) noexcept;

            void printf(const char* format, ...) noexcept;

            ConsoleIO() noexcept = default;
            ~ConsoleIO() noexcept = default;
    };

    static ConsoleIO console;
} // namespace runtime
