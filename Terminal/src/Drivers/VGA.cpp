#include "Drivers/VGA.hpp"

namespace terminal::drivers {

    void VGA::clear_screen(const uint8_t color) const noexcept {

        for (int y = 0; y < VGA_HEIGHT; y++)
            for (int x = 0; x < VGA_WIDTH; x++)
                put_char_at(
                    0x00, make_color(
                        VGAColors(color),
                        VGAColors(color)
                    ),
                    x, y
                );
    }
} // namespace terminal::drivers
