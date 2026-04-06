#include "ConsoleIO.hpp"

namespace runtime {

    void ConsoleIO::reset() noexcept {

        cursor_x = DEFAULT_CURSOR_POS;
        cursor_y = DEFAULT_CURSOR_POS;
        cursor_color = DEFAULT_COLOR;

        vga_driver.clear_screen(drivers::vga::VGAColors::BLACK);
    }

    void ConsoleIO::set_char_colors(
        const drivers::vga::VGAColors color, 
        const drivers::vga::VGAColors background) noexcept {

        // Guard against invalid color values
        constexpr uint8_t MAX_COLOR = 
            static_cast<uint8_t>(drivers::vga::VGAColors::WHITE);
    
        if (static_cast<uint8_t>(color) > MAX_COLOR ||
            static_cast<uint8_t>(background) > MAX_COLOR)
            return;

        cursor_color = drivers::vga::VGA::make_color(color, background);
    }

    void ConsoleIO::new_line() noexcept {

        cursor_x = DEFAULT_CURSOR_POS;
        cursor_y++;

        if (cursor_y >= drivers::vga::VGA_HEIGHT) [[unlikely]]
            reset();
    }

    void ConsoleIO::put_char(const char symbol) noexcept {

        if (symbol == '\n') [[unlikely]] {

            new_line();
            return;
        }

        vga_driver.put_char_at(symbol, cursor_color, cursor_x, cursor_y);
        cursor_x++;

        if (cursor_x >= drivers::vga::VGA_WIDTH) [[unlikely]]
            new_line();
    }

    void ConsoleIO::put_string(const char* message) noexcept {

        for (uint32_t i = 0; message[i] != '\0'; i++) [[likely]]
            put_char(message[i]);
    }

    void ConsoleIO::put_int(int32_t value) noexcept {

        if (value < 0) {

            put_char('-');
            value = -value;
        }

        runtime::Array<char, 12> buffer;
        uint32_t index = 0;
        while (value > 0) [[likely]] {

            buffer[index++] = '0' + (value % 10);
            value /= 10;
        }

        for (int32_t i = index - 1; i >= 0; i--) [[likely]]
            put_char(buffer[i]);
    }

    void ConsoleIO::put_uint(uint32_t value) noexcept {

        if (value == 0) [[unlikely]] {
    
            put_char('0');
            return;
        }
    
        runtime::Array<char, 12> buffer;
        uint32_t index = 0;
        while (value > 0) [[likely]] {

            buffer[index++] = '0' + (value % 10);
            value /= 10;
        }

        for (int32_t i = index - 1; i >= 0; i--) [[likely]]
            put_char(buffer[i]);
    }

    void ConsoleIO::put_hex(uint32_t value) noexcept {

        put_char('0');
        put_char('x');
        if (value == 0) [[unlikely]] {

            put_char('0');
            return;
        }

        runtime::Array<char, 9> buffer;
        uint32_t index = 0;
        while (value > 0) [[likely]] {

            uint8_t digit = value & 0xF;
            buffer[index++] = (digit < 10) ? ('0' + digit) : ('A' + digit - 10);
            value >>= 4;
        }
    
        for (int32_t i = index - 1; i >= 0; i--) [[likely]]
            put_char(buffer[i]);
    }

    void ConsoleIO::put_bin(uint32_t value) noexcept {
    
        put_char('0');
        put_char('b');
        if (value == 0) [[unlikely]] {

            put_char('0');
            return;
        }

        runtime::Array<char, 33> buffer;
        uint32_t index = 0;
        while (value > 0) [[likely]] {

            buffer[index++] = (value & 1) ? '1' : '0';
            value >>= 1;
        }

        for (int32_t i = index - 1; i >= 0; i--) [[likely]]
            put_char(buffer[i]);
    }

    void ConsoleIO::put_ptr(uint32_t value) noexcept {
    
        put_char('0');
        put_char('x');
        if (value == 0) {
    
            put_char('0');
            return;
        }
    
        runtime::Array<char, 9> pointer_buffer;
        uint32_t pointer_index = 0;
        while (value > 0) [[likely]] {

            uint8_t digit = value & 0xF;
            pointer_buffer[value++] = (digit < 10) ? ('0' + digit) : ('A' + digit - 10);
            value >>= 4;
        }
                
        for (int32_t i = pointer_index - 1; i >= 0; i--) [[likely]]
            put_char(pointer_buffer[i]);
    }

    void ConsoleIO::printf(const char* format, ...) noexcept {

        // variadic arguments list
        va_list args;
        va_start(args, format);

        for (uint32_t i = 0; format[i] != '\0'; i++) [[likely]] {

            if (format[i] != '%') [[likely]] {

                put_char(format[i]);
                continue;
            }
            i++;

            switch (format[i]) {
    
                // print Char
                case 'c': 
                    put_char(static_cast<char>(va_arg(args, int))); 
                    break;
                
                // print String
                case 's': 
                    put_string(va_arg(args, const char*)); 
                    break;
                
                // print Signed-Integer
                case 'd': 
                    put_int(va_arg(args, int32_t)); 
                    break;

                // print Unsigned-Integer
                case 'u': 
                    put_uint(va_arg(args, uint32_t)); 
                    break; 

                // print Hexadecimal 
                case 'x': 
                    put_hex(va_arg(args, uint32_t)); 
                    break;

                // print Binary
                case 'b': 
                    put_bin(va_arg(args, uint32_t)); 
                    break;
                
                // print Pointer
                case 'p': 
                    put_ptr(va_arg(args, uint32_t)); 
                    break;
                
                // print '%
                case '%': 
                    put_char('%'); 
                    break;

                default: 
                    put_char(format[i]); 
                    break;
            }
        }
    
        // clean variadic arguments list
        va_end(args);
    }
} // namespace runtime

runtime::ConsoleIO console;
