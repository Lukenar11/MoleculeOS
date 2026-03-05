# Terminal – High‑Level Text Output API (MoleculeOS)

The MoleculeOS Terminal provides a **high‑level, freestanding, deterministic** interface for text output in VGA text mode. <br>
It builds on top of the VGA driver and offers cursor management, formatted integer printing, and simple string output. <br>

The Terminal is intentionally minimal and optimized for early‑boot kernel environments where no C++ standard library is available.

---

## Terminal Overview

The Terminal subsystem handles:

- character output  
- string output  
- formatted integer printing  
- cursor movement  
- line breaks  
- screen reset  

It abstracts away the VGA hardware and exposes a clean, intuitive API for kernel developers.

The Terminal is designed to be:

- **minimal**  
- **predictable**  
- **freestanding**  
- **didactically clear**  

---

## Architecture

The Terminal is structured into three layers:

1. **VGA Driver**  
   Low‑level memory writes to `0xB8000`.

2. **Terminal State**  
   Cursor position, color, and line‑handling logic.

3. **High‑Level API**  
   `put_char`, `put_string`, `print`, `new_line`, `reset`.

This separation ensures clarity and makes the Terminal easy to extend in future MoleculeOS versions.

---

# Public API

## `reset()` – Reset Terminal State

### Purpose
Clears the screen and resets cursor and color.

### **Code**
``` cpp
    void terminal::Terminal::reset() noexcept {

        cursor_x = DEFAULT_CURSOR_POS;
        cursor_y = DEFAULT_CURSOR_POS;
        cursor_color = DEFAULT_COLOR;

        vga_driver.clear_screen(drivers::VGAColors::BLACK);
    }
```

### Explanation
- Resets internal state  
- Clears the entire VGA buffer  
- Ensures deterministic startup behavior  

---

## `new_line()` – Line Break

### **Purpose**
Moves the cursor to the next line.

### **Code**
``` cpp
    void terminal::Terminal::new_line() noexcept {

        cursor_x = DEFAULT_CURSOR_POS;
        cursor_y++;

        if (cursor_y >= drivers::VGA_HEIGHT)
            reset();
    }
```

### Explanation
- Inserts a line break at the current cursor position

---

## `put_char(char symbol)` – Write a Character

### **Purpose**
Outputs a single character at the current cursor position.

### **Code**
``` cpp
    void terminal::Terminal::put_char(const char symbol) noexcept {

        if (symbol == '\n') {

            new_line();
            return;
        }

        vga_driver.put_char_at(symbol, cursor_color, cursor_x, cursor_y);
        cursor_x++;

        if (cursor_x >= drivers::VGA_WIDTH)
            new_line();
    }
```

### Explanation
- Handles newline characters  
- Writes via VGA driver  
- Updates cursor position  

---

## `put_string(const char* message)` – Write a C‑String

### **Purpose**
Outputs a null‑terminated string.

### **Code**
``` cpp
    void ternimal::Terminal::put_string(const char* message) noexcept {

        for (uint32_t i = 0; message[i] != '\0'; i++)
            put_char(message[i]);
    }
```

### Explanation
- Iterates through each character  
- Uses `put_char()` internally  

---

## `print(int32_t value, PrintFormat format)` – Formatted Integer Output

### **Purpose**
Prints integers in decimal, hexadecimal, or binary format.

### Code (excerpt)
``` cpp
    void ternimal::Terminal::print(int32_t value, const PrintFormat format) noexcept {

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
                uint32_t index = 0;
                while (value > 0) {

                    buffer[index++] = '0' + (value % 10);
                    value /= 10;
                }

                for (int32_t i = index - 1; i >= 0; i--)
                    put_char(buffer[i]);
                break;
            }

            case PrintFormat::HEX: {

                put_char('0');
                put_char('x');

                for (int32_t i = (sizeof(unsigned_value) << 3) - 4; i >= 0; i -= 4) {

                    uint32_t digit = (unsigned_value >> i) & 0x0F;
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

                for (int32_t i = (sizeof(unsigned_value) << 3) - 1; i >= 0; i--) {

                    uint32_t bit = (unsigned_value >> i) & 1;
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
```

### Explanation
- No dynamic memory  
- No recursion  
- No standard library

---

# Example Usage

```cpp
    void test_terminal() {

        terminal::Terminal terminal;
        terminal.reset();

        terminal.put_string("Hello from C++ kernel!\n");

        terminal.print(-12345, terminal::PrintFormat::INTEGER);
        terminal.new_line();

        terminal.print(12345, terminal::PrintFormat::INTEGER);
        terminal.new_line();

        terminal.print(0xDEADBEEF, terminal::PrintFormat::HEX);
        terminal.new_line();

        terminal.print(0b101010, terminal::PrintFormat::BINARY);
        terminal.new_line();

        terminal.put_char('H');
        terminal.put_char('e');
        terminal.put_char('l');
        terminal.new_line();
        terminal.put_char('l');
        terminal.new_line();
        terminal.put_char('o');
        terminal.new_line();
    }
```

---

# Design Philosophy

The Terminal follows the MoleculeOS principles:

- **Minimalism:** only essential features  
- **Determinism:** predictable behavior, no hidden state  
- **Freestanding:** no standard library  
- **Clarity:** explicit operations  

---

# Future Extensions

Potential improvements for MoleculeOS v2:

- scrolling instead of full reset  
- cursor blinking   
- terminal buffer  
- escape sequences  
- integration into a window manager  

---

## Summary

- offers a freestanding high‑level API for text output in VGA text mode
- completely encapsulates the VGA driver and manages cursor, color, and line breaks
- supports character, string, and formatted integer output (decimal, hexadecimal, binary)
- works without standard library, without dynamic memory, and without exceptions
- guarantees deterministic behavior through simple, transparent logic
- uses only direct VGA write operations via the VGA driver
- implements a minimal, robust core functionality for early kernel output
