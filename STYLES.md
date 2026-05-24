# MoleculeOS – STYLE.md (v1.0)
MoleculeOS Coding & Naming Style Guide
Valid for all kernel, driver, runtime, and for C, C++ & Assembly

---
---

## 1. Naming Conventions

### 1.1 Types (Classes, Structs, Enums, Enum Classes)
**Pascal_Snake_Case**

Examples:
- `PIC_Mapping`
- `Register_Dump`
- `IDT_INIT_Entry`
- `Shell`

---

### 1.2 Namespaces
**lowercase**

Examples:
- `drivers`
- `runtime`
- `kernel`
- `ps2`

---

### 1.3 Variables, Functions, Methods, Objects
**snake_case**

Examples:
- `cursor_x`
- `text_output`
- `load_idt()`
- `kernel_system_sleep()`

---

### 1.4 Constants, Macros, enum values, constexpr
**SCREAMING_SNAKE_CASE**

Examples:
- `NULL`
- `VGA_TEXMODE_SCREEN_WIDTH`
- `VGA_TEXMODE_SCREEN_HEIGHT`
- `LIGHT_MAGENTA`

---

### 1.5 Files
**snake_case**

Examples:
- `kernel_main.cpp`
- `text_output.hpp`
- `kernel_system_sleep.asm`

---

### 1.6 Folders
**lowercase**

Examples:
- `drivers/`
- `runtime/`
- `keyboardin/`
- `interpreter/`

---
---

# 2. Formatting Conventions

## 2.1 Indentation

- Use **tabs** set to **4 spaces** for indentation.  
- Do not mix methods (do not use spaces for indentation).  
- One tab for each new block level.

---

## 2.2 Brace Style (Hybrid: Allman + K&R)

### Outer Blocks → Allman Style

Example:

```cpp
    namespace shell
    {
        class Shell {
        private:
            /* ... */
            /* ... */

        public:
            void step() noexcept;

            /* ... */
            /* ... */
        };
    }
```

### Inner Blocks → K&R Style

Examples:

```cpp
    void Shell::step() noexcept
    {
        while (keyboard_input.has_pending_scancode()) {
            char key = keyboard_input.get_key();
            if (!key)
    			continue;

            /* ... */
            /* ... */
        }
    }
```

```cpp
    namespace shell
    {
        void Shell::step() noexcept {
            while (keyboard_input.has_pending_scancode()) {
                char key = keyboard_input.get_key();
        		if (!key)
                    continue;

                /* ... */
                /* ... */
            }
        }
    }
```
---

## 2.3 Optional Braces

### Braces may be omitted if:

- The block contains **only a single line**  
- The line is **not too long**  
- There are **no nested conditions**  
- There are **no side effects** that could cause confusion

Example (allowed):

```cpp
    if (!key)
        continue;
```

Example (not allowed):

```cpp
    if (condition)
        do_something(); do_something_else();   // PROHIBITED
```

### **Exception:**  
If there are many single-line if statements one below the other, single-line blocks are allowed to improve readability.

```cpp
    if (condition) do_something();
    if (condition) do_something_else();
    if (condition) meep();
    if (condition) hello_world();
```

---

## 2.4 Switch-Case Formatting

- Single-line cases are allowed if the logic remains on **a single line**  
- Do not exceed the character limit  
- No fall-through without an explicit comment

Example (single-line):

```cpp
    switch (scancode) {
    case static_cast<char>(Spechial_keys::NEW_LINE): return '\n';
    case static_cast<char>(Spechial_keys::SPACE): return ' ';
    default: return 0;
    }
```

Example (multi-line):

```cpp
    switch (symbol) {
    case '\r':
        /* ... */
        /* ... */
        break;
            
    case '\b':
        /* ... */
        /* ... */
        break;

    case '\t':
        /* ... */
        /* ... */
        break;
    
    case '\n':
        /* ... */
        /* ... */
        break;

    default:
        /* ... */
        /* ... */
        break;
    }
```

---

## 2.5 Line Length

- **Soft Limit:** 80 characters  
- **Hard Limit:** 95 characters  
- If exceeded → Wrap lines

## 2.6 Includes

### Order of Includes

1. Native MoleculeOS headers
   (in the order they are needed in the code)

2. Library headers
   (e.g., `<stdint.h>`, `<stddef.h>`, `<array>` — also in the order of use)

### Rule: “First use → first include”

If an array is used first in the code → include `<array>` first  
If a `uint32_t` appears afterward → include `<stdint.h>` afterward

Example:

```cpp
    #include "drivers/vga/textmode/include/textmode.hpp"   // needed first
    #include "kernel/include/system/kernel_system_sleep.h" // needed afterward
    #include <array>     // used first in the code
    #include <stdint.h>  // used afterward
```

---

## 2.7 Header Guards

- **C++ Header:** `#pragma once`  
- **C Header:** classic header guards

C example:

```c
    #ifndef KERNEL_SYSTEM_SLEEP_H
    #define KERNEL_SYSTEM_SLEEP_H

    /* ... */
    /* ... */

    #endif // KERNEL_SYSTEM_SLEEP_H
```

C++ example:

```cpp
    #pragma once

    /* ... */
    /* ... */
```

---

## 2.8 Function Signatures & Line Breaking

### Break function signatures as follows:

- Break parameters into separate lines  
- Place `const noexcept` on a new line if the character limit has been exceeded.

Examples:

```cpp
    namespace shell
    {
        void Shell::draw_user_cursor_with_color(drivers::vga::VGA_Textmode_Colors foreground,
                                                drivers::vga::VGA_Textmode_Colors background)
                                                const noexcept {
            /* ... */
            /* ... */
        }
    }
```

```cpp
    namespace kernel::system
    {
        void panic(const char* error_message, 
                   const char* troubleshooting_message) noexcept {
            /* ... */
            /* ... */
        }
    }
```

---

## 2.9 Multi-Line Expressions

### Ternary Operator

- **Align** the question mark and colon one below the other when a character limit has been reached
- The condition before the question mark must always be enclosed in parentheses

Example (with character limit exceeded):

```cpp
    char character = (shift_is_pressed)
                     ? us_qwerty_shift_key_mapping[scancode]
                     : us_qwerty_std_key_mapping[scancode];
```

Examples (without exceeding the character limit):

```cpp
    buffer[i++] = (number < 10) ? ('0' + number) : ('A' + number - 10);
```

```cpp
    return (number < 10) ? '0' : 'A';
```

### Function Calls with Long Arguments

Example (with long arguments):

```cpp
    kernel::system::panic(
        "Unexpected return from the \"kernel_main\" scheduler main loop",
        "This should never happen.\nPlease report this to the developer."
    );
```


Example (with short arguments):

```cpp
    runtime::text_output.put_string(“Hello, World!\n”);
```

---

# 2.10 Label Alignment (case:, default:, labels)

All **label-like constructs** (e.g., `case`, `public:`, `label:` in C/C++/ASM) must be **aligned in the same column** as the block to which they belong.

This improves visual structure and makes switch blocks extremely easy to scan.

### Allowed (correctly aligned):

```cpp
    switch (symbol) {
    case '\r':
    	/* ... */
        /* ... */
    	break;

    case '\b':
    	/* ... */
        /* ... */
    	break;

    case '\t':
    	/* ... */
        /* ... */
    	break;

    case '\n':
    	/* ... */
        /* ... */
    	break;

    default:
    	/* ... */
        /* ... */
    	break;
    }
```

```cpp
    namespace namespacename
    {
        class Class_Name {
        private:
            /* ... */
            /* ... */

        public:
            /* ... */
            /* ... */
        };
    }// namespace namespacename
```

```cpp
    /*
    LICENSE:
        Copyright (c) 2026 Lukenar11 (Luke Matthes)
        MIT Licensed
        https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

    DESCRIPTION:
        This file contains the kernel entry point "kernel_main," which
        is called directly after the bootloader hands control over to the kernel.

        The function initializes core subsystems such as the IDT,
        performs a simple interrupt- and endless-loop-driven scheduling, and starts the shell.

    NOTES:
        This "kernel_main" function must never return. If execution reaches the end of
        "kernel_main," a kernel panic is triggered to prevent undefined behavior.

        Since this function is called by an assembly routine,
        it is declared as "extern C" to ensure compatibility.
    */
```

---

### Why this is important

- Labels are **structural elements**, not indented statements.  
- They mark **jump targets** and must therefore stand out visually.  
- The code becomes much easier to read, especially in large switch blocks.

### Also applies to C/C++ labels:

```cpp
    loop_start:
    	/* ... */
        /* ... */
    	goto loop_start;
```

### Also applies to assembly labels:

```asm
    load_gdt:
        lgdt [gdt_descriptor]

        mov ax, KERNEL_DATA_SEGMENT_SELECTOR
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax
        mov ss, ax

        jmp 0x08:.flush

    .flush:
        ret
```

---
