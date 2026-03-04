#include "Terminal.hpp"

namespace terminal {

    void Terminal::reset() noexcept {

        cursor_x = DEFAULT_CURSOR_POS;
        cursor_y = DEFAULT_CURSOR_POS;
        cursor_color = DEFAULT_COLOR;

        vga_driver.clear_screen(drivers::VGAColors::BLACK);
    }

    void Terminal::new_line() noexcept {

        cursor_x = DEFAULT_CURSOR_POS;
        cursor_y++;

        if (cursor_y >= drivers::VGA_HEIGHT)
            reset();
    }

    void Terminal::put_char(const char symbol) noexcept {

        if (symbol == '\n') {

            new_line();
            return;
        }

        vga_driver.put_char_at(symbol, cursor_color, cursor_x, cursor_y);
        cursor_x++;

        if (cursor_x >= drivers::VGA_WIDTH)
            new_line();
    }

    void Terminal::put_string(const char* message) noexcept {

        for (int i = 0; message[i] != '\0'; i++)
            put_char(message[i]);
    }
    
    void Terminal::print(int value, const PrintFormat format) noexcept {

        const uint32_t unsigned_value = uint32_t(value);
        bool started = false;
        switch (format) {

            case PrintFormat::INTEGER: {

                if (value == 0) {

                    put_char('0');
                    break;
                }

                if (value < 0) {

                    put_char('-');
                    value = -value;
                }

                atom::Array<char, 12> buffer{};
                int index = 0;
                while (value > 0) {

                    buffer[index++] = '0' + (value % 10);
                    value /= 10;
                }

                for (int i = index - 1; i >= 0; i--)
                    put_char(buffer[i]);
                break;
            }

            case PrintFormat::HEX: {

                put_char('0');
                put_char('x');

                for (int i = (sizeof(unsigned_value) << 3) - 4; i >= 0; i -= 4) {

                    int digit = (unsigned_value >> i) & 0x0F;
                    if (digit != 0 || started || i == 0) {

                        started = true;
                        put_char((digit < 10) ? digit + '0' : (digit - 10) + 'A');
                    }
                }
                break;
            }

            case PrintFormat::BINARY: {

                put_char('0');
                put_char('b');

                for (int i = (sizeof(unsigned_value) << 3) - 1; i >= 0; i--) {

                    int bit = (unsigned_value >> i) & 1;
                    if (bit != 0 || started || i == 0) {

                        started = true;
                        put_char(bit ? '1' : '0');
                    }
                }
                break;
            }

            default:
                put_string("Invalid print format");
                break;
        }
    }
} // namespace terminal
