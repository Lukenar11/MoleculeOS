#pragma once

#include "utils/helpers.hpp"

namespace drivers::vga {

    // VGA-Driver
    class VGA final {

        public:
            static constexpr uint8_t make_color(
                const VGAColors foreground, const VGAColors background) noexcept {

                return static_cast<uint8_t>(background) << 4 | static_cast<uint8_t>(foreground);
            }

            static constexpr uint16_t make_symbol_entry(
                const char symbol, const uint8_t color) noexcept {
                
                return static_cast<uint16_t>(color) << 8 | static_cast<uint16_t>(symbol);
            }

            void put_char_at(
                const char symbol, 
                const uint8_t color, 
                const int32_t x, 
                const int32_t y) const noexcept;

            void clear_screen(const VGAColors color) const noexcept;

            VGA() noexcept = default;
            ~VGA() noexcept = default;
    };
} // namespace drivers::vga
