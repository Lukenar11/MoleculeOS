#include "ConsoleIO.hpp"

namespace runtime {

    void ConsoleIO::reset() noexcept {

        cursor_x = 0;
        cursor_y = 0;

        drivers::vga::vga_driver.clear_screen(drivers::vga::VGAColors::BLACK);
    }

    void ConsoleIO::set_char_colors(
        const drivers::vga::VGAColors& color,
        const drivers::vga::VGAColors& background) noexcept {

        cursor_color = drivers::vga::vga_driver.make_color(color, background);
    }

    void ConsoleIO::new_line() noexcept {

        cursor_x = 0;
        cursor_y++;

        if (cursor_y >= drivers::vga::VGA_HEIGHT) [[unlikely]]
            cursor_y = drivers::vga::VGA_HEIGHT - 1;
    }

    void ConsoleIO::put_base(uint32_t value, const uint32_t base) noexcept {

        if (value == 0) [[unlikely]] {

            put_char('0');
            return;
        }

        if (base < 2 || base > 16) [[unlikely]]
            return;

        runtime::Array<char, 32> buffer;
        uint32_t i = 0;

        while (value) [[likely]] {

            const uint8_t number = value % base;
            buffer[i++] = (number < 10) ? ('0' + number) : ('A' + number - 10);
            value /= base;
        }

        while (i--) [[likely]]
            put_char(buffer[i]);
    }

    void ConsoleIO::put_char(const char symbol) noexcept {

        if (symbol == '\n') [[unlikely]] {

            new_line();
            return;
        }

        drivers::vga::vga_driver.put_char_at(symbol, cursor_color, cursor_x, cursor_y);
        cursor_x++;
        if (cursor_x >= drivers::vga::VGA_WIDTH) [[unlikely]]
            new_line();
    }

    void ConsoleIO::put_string(const char* message) noexcept {

        while (*message) [[likely]]
            put_char(*message++);
    }

    void ConsoleIO::put_int(int32_t value) noexcept {

        if (value < 0) [[unlikely]] {

            put_char('-');
            value = static_cast<uint32_t>(-(value + 1)) + 1;
            put_uint(value);
            return;
        }

        put_uint(value);
    }

    void ConsoleIO::put_uint(uint32_t value) noexcept {

        if (value == 0) [[unlikely]] {

            put_char('0');
            return;
        }

        runtime::Array<char, 12> buffer;
        uint32_t i = 0;

        while (value) [[likely]] {
        
            buffer[i++] = '0' + (value % 10);
            value /= 10;
        }

        while (i--) [[likely]]
            put_char(buffer[i]);
    }

    void ConsoleIO::put_hex(const uint32_t value) noexcept {

        put_char('0'); 
        put_char('x');
        put_base(value, 16);
    }

    void ConsoleIO::put_bin(const uint32_t value) noexcept {

        put_char('0'); 
        put_char('b');
        put_base(value, 2);
    }

    void ConsoleIO::put_ptr(const uintptr_t value) noexcept {
        
        put_char('0'); 
        put_char('x');
        put_base(value, 16);
    }

    void ConsoleIO::printf(const char* format, ...) noexcept {

        va_list args;
        va_start(args, format);

        while (*format) {

            if (*format != '%') [[likely]] {

                put_char(*format++);
                continue;
            }

            format++;

            switch (*format) {

                case 'c': {

                    const char argument = static_cast<char>(va_arg(args, int));
                    put_char(argument);
                    break;
                }

                case 's': {

                    const char* argument = va_arg(args, const char*);
                    put_string(argument);
                    break;
                }

                case 'd': {

                    const int32_t argument = static_cast<int32_t>(va_arg(args, int));
                    put_int(argument);
                    break;
                }

                case 'u': {

                    const uint32_t argument = va_arg(args, uint32_t);
                    put_uint(argument);
                    break;
                }

                case 'x': {

                    const uint32_t argument = va_arg(args, uint32_t);
                    put_hex(argument);
                    break;
                }

                case 'b':{

                    const uint32_t argument = va_arg(args, uint32_t);
                    put_bin(argument);
                    break;
                }

                case 'p':{

                    const uintptr_t argument = 
                        reinterpret_cast<uintptr_t>(
                            va_arg(args, void*)
                        );
                    put_ptr(argument);
                    break;
                }

                case '%':
                    put_char('%');
                    break;

                default:
                    put_char(*format);
                    break;
            }

            format++;
        }

        va_end(args);
    }

    ConsoleIO console;
} // namespace runtime
