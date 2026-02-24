# Terminal Driver – VGA (Textmode Video Output)

The MoleculeOS VGA driver provides a **minimal**, **deterministic**, and **fully freestanding** interface <br>
to the classic VGA text mode located at physical address `0xB8000`. <br>

It forms the foundation of the MoleculeOS Terminal subsystem and enables:

- writing characters to arbitrary screen positions  
- generating VGA color attributes  
- constructing 16‑bit VGA entries  
- clearing the entire screen  

The driver is intentionally small and optimized for early‑boot kernel environments.

---

## VGA Text Mode Overview

The VGA text buffer resides at:

```
    0xB8000
```

---

Each cell consists of **16 bits**:

| Bits | Meaning |
|------|---------|
| 0–7  | ASCII character |
| 8–15 | Color attribute (foreground + background) |

---

The screen layout is fixed:

- **80 columns**
- **25 rows**
- **2000 total cells**

The driver writes directly into this memory region using a `volatile uint16_t*`.

---

## Color Model (`VGAColors`)

The VGA color attribute is an 8‑bit value:

- lower 4 bits: **foreground color**
- upper 4 bits: **background color**

Example from the driver:

``` cpp
    enum class VGAColors : uint8_t {

        BLACK = 0x00,
        BLUE = 0x01,
        GREEN = 0x02,
        CYAN = 0x03,
        RED = 0x04,
        MAGENTA = 0x05,
        BROWN = 0x06,
        LIGHT_GREY = 0x07,
        DARK_GREY = 0x08,
        LIGHT_BLUE = 0x09,
        LIGHT_GREEN = 0x0A,
        LIGHT_CYAN = 0x0B,
        LIGHT_RED = 0x0C,
        LIGHT_MAGENTA = 0x0D,
        YELLOW = 0x0E,
        WHITE = 0x0F
    };
```

These values map directly to VGA hardware color codes.

---

## VGA Driver – Class Overview

The `terminal::drivers::VGA` class provides the minimal set of operations required for text‑mode output.

All methods are:

- `inline` where appropriate  
- `noexcept` 
- some `constexpr`
- freestanding‑compatible  
- fully deterministic  

The driver does **not** depend on the C++ standard library.

---

## `make_color` – Construct a VGA Color Attribute

### **Purpose**

Creates an 8‑bit VGA color attribute from a foreground and background color.

Used by:

- text output  
- screen clearing  
- terminal formatting  

### Code

``` cpp
    inline constexpr uint8_t make_color
        (const VGAColors foreground, const VGAColors background) 
            const noexcept {return (uint8_t(background) << 4) | uint8_t(foreground);}
```

### Explanation

- Background color is shifted into the upper 4 bits  
- Foreground color occupies the lower 4 bits  

This matches the VGA hardware format exactly.

---

## `make_entry` – Construct a 16‑bit VGA Cell

### Purpose

Combines a character and a color attribute into a single 16‑bit VGA entry.

Used by:

- `put_char_at`
- `clear_screen`
- terminal rendering logic

### Code

``` cpp
    inline constexpr uint16_t make_entry
        (const char symbol, const uint8_t color) 
            const noexcept {return (uint16_t(color) << 8) | uint8_t(symbol);}
```

### Explanation

- Color occupies the upper byte  
- ASCII character occupies the lower byte  

This is the exact memory layout expected by VGA text mode.

---

## `put_char_at` – Write a Character to the Screen

### Purpose

Writes a single character to a specific **(x, y)** position on the screen.

### Code

``` cpp
    inline void put_char_at
        (const char symbol, const uint8_t color, const int x, const int y) 
            const noexcept {VGA_BUFFER[y * VGA_WIDTH + x] = make_entry(symbol, color);}
```

### Explanation

Index calculation:

```
    index = x + (y * 80)
```

This directly maps to the VGA text buffer layout.

---

## `clear_screen` – Fill the Entire Screen

### Purpose

Clears the entire VGA text buffer by writing a space `' '` with a given color attribute.

Used for:

- terminal reset  
- kernel boot messages  
- debugging output  

### Code

```cpp
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
```

### Explanation

- Precomputes the VGA entry for performance  
- Writes 2000 cells  
- Fully deterministic and freestanding  

---

## VGA - Memory Layout

```
    +---------------------------------------------------------------+
    | Row 0                                                         |
    +---------+---------+---------+---------+---------+-------------+
    | (0,0)   | (1,0)   | (2,0)   |  ...    | (79,0)  |             |
    | 0xB8000 | 0xB8002 | 0xB8004 |         |  ...    |             |
    +---------+---------+---------+---------+---------+-------------+

    +---------------------------------------------------------------+
    | Row 1                                                         |
    +---------+---------+---------+---------+---------+-------------+
    | (0,1)   | (1,1)   | (2,1)   |  ...    | (79,1)  |             |
    | 0xB8000 + 160     | 0xB8000 + 162     | ...     |             |
    +---------+---------+---------+---------+---------+-------------+

    ...

    +---------------------------------------------------------------+
    | Row 24                                                        |
    +---------+---------+---------+---------+---------+-------------+
    | (0,24)  | (1,24)  | (2,24)  |  ...    | (79,24) |             |
    | 0xB8000 + 24*160  | ...     |         |         |             |
    +---------+---------+---------+---------+---------+-------------+
```

Each entry is a 16‑bit value written directly into the VGA buffer.

---

## Example: Usage in the Kernel

```cpp
    const char text_message[] = "Hello from C++ kernel!";
    terminal::drivers::VGA vga;

    // puts(text_message);
    for (int x = 0; x < (sizeof(text_message) / sizeof(char)); x++)
        vga.put_char_at(
            text_message[x], 
            uint8_t(terminal::drivers::VGAColors::GREEN), 
            x, 
            18
        );
```

This prints a green message centered horizontally on row 18.

---

## Design Philosoph

The VGA driver follows the MoleculeOS principles:

- **Minimalism:** Only essential operations  
- **Determinism:** No dynamic memory, no exceptions  
- **Freestanding:** No dependencies on the C++ runtime  
- **Performance:** Inline functions, direct memory access  
- **Didactic clarity:** Every operation is explicit and transparent  

---

## Complete code

``` cpp
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
```

---

## Header File `VGA.hpp`

``` cpp
    #pragma once

    #include <stdint.h>

    namespace terminal::drivers {

        inline volatile uint16_t* const VGA_BUFFER = (volatile uint16_t*)0xB8000;
        constexpr int VGA_WIDTH = 80;
        constexpr int VGA_HEIGHT = 25;

        // char/screen Colors
        enum class VGAColors : uint8_t {

            BLACK = 0x00,
            BLUE = 0x01,
            GREEN = 0x02,
            CYAN = 0x03,
            RED = 0x04,
            MAGENTA = 0x05,
            BROWN = 0x06,
            LIGHT_GREY = 0x07,
            DARK_GREY = 0x08,
            LIGHT_BLUE = 0x09,
            LIGHT_GREEN = 0x0A,
            LIGHT_CYAN = 0x0B,
            LIGHT_RED = 0x0C,
            LIGHT_MAGENTA = 0x0D,
            YELLOW = 0x0E,
            WHITE = 0x0F
        };

        // VGA-Driver
        class VGA {

            public:

                inline constexpr uint8_t make_color
                    (const VGAColors foreground, const VGAColors background) 
                        const noexcept {return (uint8_t(background) << 4) | uint8_t(foreground);}

                inline constexpr uint16_t make_entry
                    (const char symbol, const uint8_t color) 
                        const noexcept {return (uint16_t(color) << 8) | uint8_t(symbol);}

                inline void put_char_at
                    (const char symbol, const uint8_t color, const int x, const int y) 
                        const noexcept {VGA_BUFFER[y * VGA_WIDTH + x] = make_entry(symbol, color);}

                void clear_screen(const uint8_t color) const noexcept;

                VGA() noexcept = default;
                ~VGA() noexcept = default;
        };

    } // namespace terminal::drivers
```

---

## Future Extensions

The VGA driver forms the foundation for the Terminal subsystem.  
Possible extensions include:

- cursor control  
- scrolling  
- formatted text output  
- color themes  
- terminal abstraction layer  

---

## Summary

The MoleculeOS VGA driver provides a minimal, deterministic, and freestanding interface to VGA text mode.  

It offers:

- color construction  
- VGA entry construction  
- character output  
- full‑screen clearing  

Through its simplicity and direct memory access, <br>
it integrates seamlessly into the MoleculeOS architecture and <br>
serves as the basis for higher‑level terminal functionality.
