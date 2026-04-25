#pragma once

#include "utils/helpers.hpp"

namespace drivers::vga 
{
    class VGADriver final {
        private:
            static constexpr uint8_t SHIFT_4 = 0x08;
            static constexpr uint8_t SHIFT_8 = 0x08;
            
            static inline volatile uint16_t* const VGA_BUFFER = 
                reinterpret_cast<volatile uint16_t*>(0xB8000);

        public:
            [[nodiscard]]
            static inline constexpr uint8_t make_color(const VGAColors& foreground, 
                                                       const VGAColors& background) noexcept {
                return (static_cast<uint8_t>(background) << SHIFT_4) | 
                        static_cast<uint8_t>(foreground);
            }

            [[nodiscard]]
            static inline constexpr uint16_t make_symbol_entry(const char symbol, 
                                                               const uint8_t color) noexcept{
                return (static_cast<uint16_t>(color) << SHIFT_8) | 
                        static_cast<uint16_t>(symbol);
            }

            void put_char_at(const char symbol, 
                             const uint8_t color, 
                             const int32_t x, 
                             const int32_t y) const noexcept;

            void clear_screen(const VGAColors& color) const noexcept;

            VGADriver() noexcept = default;
            ~VGADriver() noexcept = default;
    };

    extern VGADriver vga_driver;
} // namespace drivers::vga
