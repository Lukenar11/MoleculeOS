# MoleculeOS – ConsoleIO (Textausgabe‑Subsystem)
ConsoleIO is the central text output subsystem of MoleculeOS. <br> 
It provides an **easy-to-use, hardware-agnostic API** that internally uses the VGA driver to output text in **80×25 text mode**. <br>

ConsoleIO is deliberately minimal and forms the basis for:

- Kernel debug output
- Error messages (e.g., in `isr_common_handler`)
- Simple user interaction
- Early runtime functions like `printf`

ConsoleIO is completely heap-free, deterministic, and suitable for kernel use.

---

## Purpose of ConsoleIO

ConsoleIO wraps the VGA driver and provides a **convenient, C-like API, with OOP style**:

- Output characters
- Output strings
- Output formatted text (`printf`)
- Set text colors
- Manage cursor position

ConsoleIO does **not** handle complex tasks such as:

- Scrolling
- Cursor blinking
- Graphics mode
- Unicode

It is a deliberately simple text I/O subsystem.

---

## Why ConsoleIO Exists

The VGA driver operates at a very low level:

- direct write access to `0xB8000`
- manually setting color attributes
- managing cursor position yourself

ConsoleIO abstracts these details and provides a **clean runtime API** that the kernel can use everywhere.

The result:

- less code duplication
- clearer kernel structure
- simple debug output
- consistent text display

---

## ConsoleIO Architecture Overview

ConsoleIO consists of:

- an **internal cursor** (`cursor_x`, `cursor_y`)
- a **current text color** (`cursor_color`)
- a **VGA driver object** (`vga_driver`)
- a new line function (`new_line`)
- simple output functions (`put_char`, `put_string`)
- a small but functional `printf` parser

ConsoleIO is completely **hardware-agnostic** since all hardware access goes through the VGA driver.

---

## Memory Layout Interaction

ConsoleIO writes exclusively to the VGA text mode memory:

``` text
    0x000B8000 – 0x000B8FA0
```

This area is:

- 80×25 characters
- 2 bytes per character (ASCII + color)
- directly visible on the screen

ConsoleIO manages:

- Cursor position
- Line breaks
- Screen reset

---

# `reset` – Clear screen and reset cursor

### **Purpose**

`reset()` sets the console to a defined initial state.
It is typically used at kernel startup or after an error.

The function:

- clears the entire VGA text mode screen
- sets the cursor to position (0,0)
- restores the default color
- ensures a clean output environment

### **Code**

```cpp
    void ConsoleIO::reset() noexcept {

        cursor_x = DEFAULT_CURSOR_POS;
        cursor_y = DEFAULT_CURSOR_POS;
        cursor_color = DEFAULT_COLOR;

        vga_driver.clear_screen(drivers::vga::VGAColors::BLACK);
    }
```

---

## `new_line` – Move cursor to next line

### **Purpose**

`new_line()` encapsulates the behavior of a line break.
Instead of duplicating `'n'` special logic everywhere, this function bundles:

- Set the cursor to the beginning of the line
- Move one line down

It is the internal basis for line breaks in `put_char` and `printf`.

### **Code**

```cpp
    void ConsoleIO::new_line() noexcept {

        cursor_x = DEFAULT_CURSOR_POS;
        cursor_y++;

        if (cursor_y >= drivers::vga::VGA_HEIGHT) [[unlikely]]
            reset();
    }
```

---

# `set_char_colors` – Set text and background color

### **Purpose**

Sets the current text color, which applies to all future output.  
The color is not applied retroactively — only new characters use it.  

Important for:  

- Error messages  
- Colored debug output  
- Visual structuring

### **Code**

```cpp
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
```

---

# `put_char` – Write a single character

### **Purpose**

Outputs a single character at the current cursor position. <br>
This is the basic output operation on which all other functions are built. <br>

Special cases:

- `'n'` → line break

### **Code**

```cpp
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
```

---

# `put_string` – Write a null‑terminated string

### **Purpose**

Outputs a null-terminated C string, character by character. <br>
This is the standard function for simple text output. <br>

Frequently used for:

- Debug output
- Error messages

### **Code**

```cpp
    void ConsoleIO::put_string(const char* message) noexcept {

        for (uint32_t i = 0; message[i] != '\0'; i++) [[likely]]
            put_char(message[i]);
    }
```

---

# `printf` – Formatted kernel output

### **Purpose**

`printf` is the most important output function in the kernel.  
It allows formatted text.

Supported format specifiers:

- `%c` – character
- `%s` – string
- `%d` – signed int
- `%u` – unsigned int
- `%x` – hexadecimal
- `%b` – binary
- `%p` – pointer
- `%%` – percent sign

### **Code**

```cpp
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
                case 'c': {
    
                    const char char_argument = static_cast<char>(va_arg(args, int));
                    put_char(char_argument);
                    break;
                }

                // print String
                case 's': {
    
                    const char* string_argument = va_arg(args, const char*);
                    put_string(string_argument);
                    break;
                }

                // print Signed-Integer
                case 'd': {
    
                    int32_t value = va_arg(args, int32_t);
                    if (value < 0) {

                        put_char('-');
                        value = -value;
                    }

                    runtime::Array<char, 12> buffer{};
                    uint32_t index = 0;
                    while (value > 0) [[likely]] {

                        buffer[index++] = '0' + (value % 10);
                        value /= 10;
                    }

                    for (int32_t i = index - 1; i >= 0; i--) [[likely]]
                        put_char(buffer[i]);
                    break;
                }

                // print Unsigned-Integer
                case 'u': {
    
                    uint32_t unsigned_value = va_arg(args, uint32_t);
                    if (unsigned_value == 0) [[unlikely]] {

                        put_char('0');
                        break;
                    }
    
                    runtime::Array<char, 12> buffer{};
                    uint32_t index = 0;
                    while (unsigned_value > 0) [[likely]] {

                        buffer[index++] = '0' + (unsigned_value % 10);
                        unsigned_value /= 10;
                    }

                    for (int32_t i = index - 1; i >= 0; i--) [[likely]]
                        put_char(buffer[i]);
                    break;
                }

                // print Hexadecimal
                case 'x': {

                    put_char('0');
                    put_char('x');
                    uint32_t hex_value = va_arg(args, uint32_t);
                    if (hex_value == 0) [[unlikely]] {

                        put_char('0');
                        break;
                    }

                    runtime::Array<char, 9> buffer{};
                    uint32_t index = 0;
                    while (hex_value > 0) [[likely]] {

                        uint8_t digit = hex_value & 0xF;
                        buffer[index++] = (digit < 10) ? ('0' + digit) : ('A' + digit - 10);
                        hex_value >>= 4;
                    }

                    for (int32_t i = index - 1; i >= 0; i--) [[likely]]
                        put_char(buffer[i]);
                    break;
                }

                case 'b': {

                    put_char('0');
                    put_char('b');
                    uint32_t binary_value = va_arg(args, uint32_t);
                    if (binary_value == 0) [[unlikely]] {
            
                        put_char('0');
                        break;
                    }

                    runtime::Array<char, 33> buffer{};
                    uint32_t index = 0;
                    while (binary_value > 0) [[likely]] {
            
                        buffer[index++] = (binary_value & 1) ? '1' : '0';
                        binary_value >>= 1;
                    }
                    for (int32_t i = index - 1; i >= 0; i--) [[likely]]
                        put_char(buffer[i]);
                    break;
                }
    
                case 'p': {
    
                    put_char('0');
                    put_char('x');
                    uint32_t pointer_value = va_arg(args, uint32_t);
                    if (pointer_value == 0) {
        
                        put_char('0');
                        break;
                    }
    
                    runtime::Array<char, 9> pointer_buffer{};
                    uint32_t pointer_index = 0;
                    while (pointer_value > 0) [[likely]] {

                        uint8_t digit = pointer_value & 0xF;
                        pointer_buffer[pointer_index++] = (digit < 10) ? ('0' + digit) : ('A' + digit - 10);
                        pointer_value >>= 4;
                    }
                
                    for (int32_t i = pointer_index - 1; i >= 0; i--) [[likely]]
                        put_char(pointer_buffer[i]);
                    break;
                }

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
```

---

## Example Usage

``` cpp
    #include <Runtime/CPP/ConsoleIO.hpp>

    runtime::ConsoleIO console;
    console.reset();
    console.put_string("Hello, World!");
```

---

# Summary

ConsoleIO is the central text output subsystem of MoleculeOS.
It provides a simple, deterministic, and heap-free API for:

- Character and string output
- Colored text output
- Formatted text (`printf`)
- Cursor management

ConsoleIO abstracts the VGA driver and enables the kernel to have clear, uniform, and easily understandable text output. <br>
It is intentionally kept minimal to keep complexity low and to make the core principles of OS development clearly visible.
