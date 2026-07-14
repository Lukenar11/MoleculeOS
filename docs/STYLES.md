# MoleculeOS – STYLES.md
MoleculeOS Coding & Naming Style Guide

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
- `drivers::ps2`
- `shell::commands`
- `kernel`
- `runtime`

---

### 1.3 Variables, Functions, Methods, Objects
**snake_case**

Examples:
- `file_intstrem_redict_operator_pos`
- `text_output`
- `load_idt()`
- `extract_file_instream_befor_redict_operator()`

---

### 1.4 Constants, Macros, enum values, constexpr
**SCREAMING_SNAKE_CASE**

Examples:
- `LOWEST_BIT`
- `TEXT_MODE_SCREEN_WIDTH`
- `KEYBOARD_STATUS_PORT`
- `LIGHT_MAGENTA`

---

### 1.5 Files
**snake_case**

Examples:
- `kernel_main.cpp`
- `text_output.hpp`
- `sleep.asm`

---

### 1.6 Folders
**lowercase**

Examples:
- `drivers/`
- `filesystem/`
- `keyboardin/`
- `arch/`

---

# 2. Formatting Conventions

---

## 2.1 Indentation

- Use **tabs** set to **4 spaces** for indentation.  
- Do not mix methods (do not use spaces for indentation).  
- One tab for each new block level.

---

## 2.2 Brace Style (Hybrid: Allman + K&R)

### Outer Blocks → Allman Style
### Inner Blocks → K&R Style

Examples:

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

Examples (single-line):

```cpp
    switch (scancode) {
    case static_cast<char>(Spechial_keys::NEW_LINE): return '\n';
    case static_cast<char>(Spechial_keys::SPACE): return ' ';
    default: return 0;
    }
```

```cpp
    switch (scancode) {
    case contidtion: do_something(); break;
    case contidtion: do_something_else(); break;
    default: hero_of_whatever(); break;
    }
```

---

## 2.5 Line Length

- **Soft Limit:** 80 characters  
- **Hard Limit:** 100 characters  
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
    #ifndef SLEEP_H
    #define SLEEP_H

    /* ... */
    /* ... */

    #endif // SLEEP_H
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

- Break parameters into separate lines if the character limit has been exceeded
- Place `const noexcept` on a new line if the character limit has been exceeded.

Examples:

```cpp
    namespace shell
    {
        void Shell::draw_user_cursor_with_color(drivers::vga::Text_Mode_Colors& foreground,
                                                drivers::vga::Text_Mode_Colors& background)
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
    runtime::text_output.put_string("Hello, World!\n");
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
    } // namespace namespacename
```

```cpp
    /* 
    LICENSE:
        ...
        ...

    DESCRIPTION:
        ...
        ...

    NOTES:
        ...
        ...
    */
```

---

### Why this is important

- Labels are **structural elements**, not indented statements.  
- They mark **jump targets** and must therefore stand out visually.  
- The code becomes much easier to read, especially in large switch blocks.

### Also applies to C/C++ labels:

```c
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

# 3. Project & Code Structure

---

## 3.1 Project Layout (Folder Structure)

**Rules**

All major components, such as `kernel/` and `shell`, 
must have a central API located under `runtime/apis` or `runtime/apis/arch` (if the component is architecture-dependent)
and may only use the APIs of other components.

---

## 3.2 File Layout (File Structure)

**Rules:**

- Headers → `include/`  
- Implementation → `src/`  
- Classes may contain a maximum of 15 methods
- A file may be no longer than 350 lines
- Enums, structs, and classes must be marked as `final`.
- Classes may only be used for encapsulation and for the constructor and destructor system
- A class that does not use a constructor, destructor, or both must mark them as `noexcept` and `default` in the header
- Anything that can be named (e.g., functions, variables) must not use abbreviations unless they are widely recognized 
(both in the scripting language community and in the system programming community) and should be written out clearly and self-explanatorily.
- Even if only a header is needed, you must create an empty source file containing the header comment and an include statement for the header, 
to maintain a consistent file structure.
- The empty .cpp file contains only header includes and header comments.

Every file follows this exact structure:
1. **Header comment**
2. **Preprocessors**
4. **Includes** (as explained in **2.6 Includes**) 
5. **Anonymous Namespaces** / **static** (for C code)
6. **Namespaces**
7. **Types (Classes, Structs, Enums)** 

---

### Header Comment Structure:

```cpp
    /*
    LICENSE:
        Copyright (c) [Year] [Your Name]
        MIT Licensed
        [REPO Link to LICENSE]

    DESCRIPTION:
        [Description of exactly what this code does.]

    NOTES:
        [Additional notes on the code.]
    */
```

Example:

```c
    /*
    LICENSE:
    	Copyright (c) 2026 Lukenar11 (Luke Matthes)
    	MIT Licensed
    	https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

    DESCRIPTION:
    	This is a simple Assembly routine for loading the IDT.

    	This routine uses the i386 and x86/64 instruction "lidt", 
    	which tells the CPU where the IDT is located and that it should be loaded.

    NOTES:
    	The internal implementation is located in "load_idt.asm".

        This header is written in C rather than C++
        to ensure maximum compatibility, 
        as Assembly provides a C interface, but not a C++ interface.
    */
```

```asm
    ;
    ; LICENSE:
    ;     Copyright (c) 2026 Lukenar11 (Luke Matthes)
    ;     MIT licensed
    ;     https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
    ;
    ; DESCRIPTION:
    ;     This is a simple assembly routine for loading the IDT.
    ;
    ;     This routine uses the i386 and x86/64 instruction "lidt",
    ;     which tells the CPU where the IDT is located and that it should be loaded.
    ;
    ; NOTES:
    ;     The C interface is a header file called "load_idt.h".
    ;
```

---

A practical example of all the rules:

**keyboard_input.hpp**
```cpp
    /*
    LICENSE:
        Copyright (c) 2026 Lukenar11 (Luke Matthes)
        MIT Licensed
        https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

    DESCRIPTION:
        This is a PS/2 keyboard input driver, for reading translated keypresses from the
        keyboard controller (i8042).

        The driver reads raw scancodes from the data port, applies modifier
        state such as Shift and CapsLock, and returns the resulting ASCII
        character to other components like the shell.

    NOTES:
        The function "has_pending_scancode" is intentionally in the header because 
        the compiler can see and optimize header files better than source files; 
        since this function is so small, the compiler can therefore probably inline it.
    */

    #pragma once

    #include "utils/helpers.hpp"
    #include <stdint.h>
    #include <io_arch_api.hpp>

    namespace drivers::ps2 
    {
        class Keyboard_Input final {
        private:
            static constexpr uint8_t LOWEST_BIT = 0x01;
            static constexpr uint8_t ALOWED_SCANCODE_SIZE = 128;

            static constexpr uint16_t KEYBOARD_STATUS_PORT = 0x64;
            static constexpr uint16_t KEYBOARD_DATA_PORT = 0x60;

            bool shift_is_pressed = false;
            bool capslock_is_enabled = false;

        public:
            inline bool has_pending_scancode() const noexcept {
                return rintime::byte_input(KEYBOARD_STATUS_PORT) & LOWEST_BIT;
            }

            char get_key() noexcept;

            Keyboard_Input() noexcept  = default;
 noexcept = default;
 noexcept  = default;
 noexcept = default;
            ~Keyboard_Input() noexcept  = default;
 noexcept = default;
 noexcept  = default;
 noexcept = default;
        };

        extern Keyboard_Input keyboard_input;
    } // namespace drivers::ps2
```

**helpers.hpp**
```cpp
    /*
    LICENSE:
        Copyright (c) 2026 Lukenar11 (Luke Matthes)
        MIT Licensed
        https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

    DESCRIPTION:
        This file defines the US QWERTY key mapping tables used by the
        PS/2 keyboard driver.

        The tables translate raw scancodes from the
        keyboard controller into ASCII characters, both for normal and
        Shift‑modified input.

        Additionally, this header declares the special scancode constants
        used to detect modifier keys such as Shift, CapsLock, and control
        signals like key release events.

    NOTES:
        The mapping arrays must contain exactly 128 entries to match the
        valid scancode range of the PS/2 Set 1 protocol. 

        Any deviation in size or ordering would result in incorrect character translation.
    */

    #pragma once

    #include <stdint.h>

    namespace drivers::ps2
    {
        constexpr char us_qwerty_std_key_mapping[128] = {
            0, 27, '1','2','3','4','5','6','7','8','9','0', '-','=', '\b',
            '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n', 0,
            'a','s','d','f','g','h','j','k','l',';','\'','`', 0, '\\',
            'z','x','c','v','b','n','m',',','.','/', 0, '*', 0, ' ',
        };

        constexpr char us_qwerty_shift_key_mapping[128] = {
            0, 27, '!','@','#','$','%','^','&','*','(',')','_','+', '\b',
            '\t','Q','W','E','R','T','Y','U','I','O','P','{','}','\n', 0,
            'A','S','D','F','G','H','J','K','L',':','\"','~', 0, '|',
            'Z','X','C','V','B','N','M','<','>','?', 0, '*', 0, ' ',
        };

        enum class Special_Keyboard_Keys : uint8_t {
            LEFT_SHIFT = 0x2A,
            RIGHT_SHIFT = 0x36,
            KEYBOARD_RELEASE = 0x80,
            LEFT_SHIFT_RELEASE = 0xAA,
            RIGHT_SHIFT_RELEASE = 0xB6,
            CAPSLOCK = 0x3A,
            CTRL = 0x64,
            REBOOT_COMMAND = 0xFE,
        };
    } // namespace drivers::ps2
```

---

**keyboard_input.cpp**
```cpp
    /*
    LICENSE:
        Copyright (c) 2026 Lukenar11 (Luke Matthes)
        MIT Licensed
        https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

    DESCRIPTION:
        This is a PS/2 keyboard input driver, for reading translated keypresses from the
        keyboard controller (i8042).

        The driver reads raw scancodes from the data port, applies modifier
        state such as Shift and CapsLock, and returns the resulting ASCII
        character to other components like the shell.

    NOTES:
        The function "has_pending_scancode" is intentionally in the header because 
        the compiler can see and optimize header files better than source files; 
        since this function is so small, the compiler can therefore probably inline it.
    */

    #include "keyboard_input.hpp"

    namespace drivers::ps2
    {
        char Keyboard_Input::get_key() noexcept {
            const uint8_t null_terminator = '\0';

            if (!(rintime::byte_input(KEYBOARD_STATUS_PORT) & LOWEST_BIT))
                return null_terminator;

            const uint8_t scancode = rintime::byte_input(KEYBOARD_DATA_PORT);

            switch (scancode) {
            case static_cast<uint8_t>(Special_Keyboard_Keys::LEFT_SHIFT):
            case static_cast<uint8_t>(Special_Keyboard_Keys::RIGHT_SHIFT):
                shift_is_pressed = true;
                return null_terminator;

            case static_cast<uint8_t>(Special_Keyboard_Keys::LEFT_SHIFT_RELEASE):
            case static_cast<uint8_t>(Special_Keyboard_Keys::RIGHT_SHIFT_RELEASE):
                shift_is_pressed = false;
                return null_terminator;

            case static_cast<uint8_t>(Special_Keyboard_Keys::CAPSLOCK):
                capslock_is_enabled = !capslock_is_enabled;
                return null_terminator;
            }

            if (scancode & static_cast<uint8_t>(Special_Keyboard_Keys::KEYBOARD_RELEASE))
                return null_terminator;

            if (scancode >= ALOWED_SCANCODE_SIZE)
                return null_terminator;

            char character = (shift_is_pressed) 
                             ? us_qwerty_shift_key_mapping[scancode] 
                             : us_qwerty_std_key_mapping[scancode]; 

            if (capslock_is_enabled) {
                if ((character >= 'a') && (character <= 'z'))
                    character -= 32;
                else if ((character >= 'A') && (character <= 'Z'))
                    character += 32;
            }

            return character;
        }

        Keyboard_Input keyboard_input;
    } // namespace drivers::ps2
```

**helpers.cpp**
```cpp
    /*
    LICENSE:
        Copyright (c) 2026 Lukenar11 (Luke Matthes)
        MIT Licensed
        https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

    DESCRIPTION:
        This file defines the US QWERTY key mapping tables used by the
        PS/2 keyboard driver.

        The tables translate raw scancodes from the
        keyboard controller into ASCII characters, both for normal and
        Shift‑modified input.

        Additionally, this header declares the special scancode constants
        used to detect modifier keys such as Shift, CapsLock, and control
        signals like key release events.

    NOTES:
        The mapping arrays must contain exactly 128 entries to match the
        valid scancode range of the PS/2 Set 1 protocol. 

        Any deviation in size or ordering would result in incorrect character translation.
    */

    #include "helpers.hpp"
```

---

## 3.3 Header & Source Separation

### C++ Headers **(.hpp)**

- contain **only declarations**  
- contain all includes
- contain an anonymous namespace (if you want to encapsulate data or functions that are not in the class)
- contain a namespace
- a class (unless it is a helper file)
- No logic except for trivial `inline` functions  
- must not define function pointer tables (except for `constexpr`)

### C++ Source **(.cpp)**

- Contains implementations 
- Contains an anonymous namespace (if you want to encapsulate data or functions that are not in the class)

### C Header **(.h)**

- Contains **only declarations**  
- Contains all includes
- Data is encapsulated with **static**
- No logic except for trivial `inline` functions

### C Source **(.c)**

- Contains implementations  
- Data is encapsulated with **static**

---

---

## 3.4 Assembly Structure

### Rules

- No hard-coded numbers or other values; anything that isn't immediately clear must be placed in a macro or a variable.
- Macros, labels, variables, and filenames must not contain abbreviations and should be written out in full using self-explanatory names.

Assembly files follow this structure:

1. **Header comments**
2. **External labels** (using `extern`)
3. **Global labels** (using `global`)
4. **Macros**
6. **Sections** (`section .[SectionName]`)
5. **Text section** (`section .text`)

Example:

```asm
    ;
    ; LICENSE:
    ;     Copyright (c) 2026 Lukenar11 (Luke Matthes)
    ;     MIT Licensed
    ;     https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
    ;
    ; DESCRIPTION:
    ;     This file contains the kernel entry point "_start", which is the first
    ;     executed instruction after the bootloader transfers control to
    ;     the kernel.
    ;
    ;     The routine loads the Global Descriptor Table (GDT), initializes the
    ;     kernel stack, aligns it to a 16-byte boundary, and then jumps into
    ;     the C++ high-level entry point "kernel_main".
    ;
    ; NOTES:
    ;     The internal implementation of "kernel_stack_top" 
    ;     is located in "kernel_stack.asm".
    ;     The internal implementation of "load_gdt" is located in "gdt.asm".
    ;

    extern kernel_main
    extern kernel_stack_top
    extern load_gdt
    global _start

    %define STACK_ALIGN_16_MASK 0xFFFFFFF0

    section .text
    _start:
        cli

        call load_gdt

        mov esp, kernel_stack_top
        and esp, STACK_ALIGN_16_MASK

        jmp kernel_main
```

---

## 3.5 Module Structure

Modules should follow this pattern:

```txt
    module/
    │
    ├── include/
    |   |
    |   ├─ utils/
    |   |  |
    |   |  └── *.hpp / *.h
    |   |
    │   └── *.hpp / *.h
    │
    └── src/
        |
        ├─ utils/
        |  |
        |  └── *.cpp / *.c
        |
        └── *.cpp / *.c / *.asm
```

### Rules
- Even if no helper files are needed, a `utils` folder is still created to maintain a consistent folder structure.
- All header files go in `include`.
- All source files go in `src`.
- Every header file must also have a corresponding source file, even if it is not used, to maintain a consistent file structure.

---

## 3.6 Driver Structure

Each driver is a **standalone block** that contains **various modules**:

```txt
    drivers/
    │
    ├─ ps2/
    │   │   # Classic module structure, as explained in '3.4 Module Structure'
    │   ├── keyboardin/
    │   |   │
    │   |   └── ...
    |   |   
    |   └── ...
    |
    ├─ vga/
    |   │   # Classic module structure, as explained in '3.4 Module Structure'
    |   ├── textmode/
    │   |   │
    │   |   └── ...
    |   |
    |   └── ...
    |
    └── ...
```

### Rules

- Drivers must **not** depend on one another  
- Drivers must use **only runtime**  
- Drivers must **not directly** call kernel subsystems  
- Drivers should be as **simple** as possible and serve only as a **HAL (Hardware Abstraction Layer)**
- Each new driver must be included in the file `runtime\apis\drivers_api.hpp`.
- Even internal runtime components, such as “text_output,” must use the central APIs.

---

## 3.7 Runtime Structure

```txt
    runtime/
    │   # Central APIs (e.g. "drivers_api.hpp", "drivers_api.hpp")
    ├─ apis/
    │   │   # Standard APIs for architecture-specific components (e.g. "kernel_arch_api.hpp", "io_arch_api.hpp")
    │   ├── arch/
    │   |   |   
    │   |   └── ... *.hpp / *.h
    |   |
    │   |   # Standard APIs (e.g. "drivers_api.hpp")
    │   └── ... *.hpp / *.hpp
    |
    |   # C++ Runtime (e.g. "array.hpp", "text_output.hpp")
    |   # Classic module structure, as explained in '3.4 Module Structure'
    └─ cpp/
        │   # public API
        ├── include/
        |   |
        |   ├── utils/
        |   |   |   
        |   |   └── ... 
        |   |   
        |   └── ... 
        |   
        │   # Implementation
        └── src/
            ├── utils/
            |   |   
            |   └── ... 
            |   
            └── ... 
```

### Rules

- The runtime must not access **any hardware** directly (only via `drivers/`)
- The runtime is the **lowest layer**  
- The runtime must be accessible **to all other modules**
- The APIs for the runtime-specific components must include "arch" in their names and 
  use `#ifdef` preprocessor directives to compile only the code for the respective architecture.

---

## 3.8 Kernel Structure

The kernel consists of clearly separated subsystems

### Rules

- Each subsystem has **a clearly defined area of responsibility**  
- No mixing of logic  
- No global variables except for intentionally defined kernel singletons
- As few dependencies as possible between its components
- The kernel may only access architecture-dependent kernel components using their APIs.

```txt
    kernel/
    |
    ├── include/
    |   |
    |   ├── utils/
    |   |
    |   |   # Kernel Subsystem/Komponente structure
    |   ├── heap/
    |   |   |
    |   |   ├── utils/
    |   |   |   |
    |   |   |   └── ...
    |   |   |
    |   |   └── ...
    |   |
    |   |   # Kernel Subsystems/Komponentes
    |   └── ...
    |
    └── src/
        |
        ├── utils/
        |
        |   # Kernel Subsystem/Komponente structure
        ├── idt/
        |   |
        |   ├── utils/
        |   |   |
        |   |   └── ...
        |   |
        |   └── ...
        |
        |   # Kernel Subsystems/Komponentes
        ├── ...
        |
        └── kernel_main.cpp
```

---

## 3.9 Shell Structure

The shell consists of two modules: the interpreter and the command implementations.

### Rules
- All interpreter logic goes in `shell/interpreter/` 
- All internal command implementations go in `shell/commands/`
- Each command gets a separate source and header file, named after the command
- `commands/` and `interpreter/` must not be intertwined, 
and the only link between them is the **shell_command_table** in the interpreter
- `interpreter` is responsible for providing an argument array, 
calling the respective command implementations via the **shell_command_table**, and analyzing and handling syntax errors and buffer errors.
- The command implementation is responsible for parsing the passed arguments and handling any errors.
- Must not access the hardware directly
- Must not use drivers directly
- The interpreter must not be aware of the commands' logic, except for their function pointers.
The commands must have no connection to the interpreter, except for the argument array and the **shell_command_table**

``` txt
    shell/
    |
    |   # shell command implementations (e.g. echo, list)
    |   # Classic module structure, as explained in '3.4 Module Structure' 
    ├── commands/
    |   |
    |   ├── include/
    |   |   |
    |   |   ├── utils/
    |   |   |   |
    |   |   |   └── ...
    |   |   |
    |   |   └── ...
    |   |
    |   └── src/
    |       |
    |       ├── utils/
    |       |   |
    |       |   └── ...
    |       |
    |       └── ...
    |
    |   # interpreter for the shell commands
    |   # Classic module structure, as explained in '3.4 Module Structure' 
    └── interpreter/
        |
        ├── include/
        |   |
        |   ├── utils/
        |   |   |
        |   |   └── ...
        |   |
        |   └── ...
        |
        └── src/
            |
            ├── utils/
            |
            └── ...
```

---

## 3.10 Terminal Structure

The terminal is a high-level UI that uses the keyboard driver and the shell internally.

### Rules
- It should interact as little as possible with other system components
- It does not execute or contain any logic directly in the background
- It passes commands to the shell character by character
- It does not execute commands.
- It only displays user input and forwards characters to the shell.

``` txt
    # Classic module structure, as explained in '3.4 Module Structure' 
    terminal/
    │
    ├── include/
    |   |
    |   ├─ utils/
    |   |  |
    |   |  └── ...
    |   |
    │   └── ...
    │
    └── src/
        |
        ├─ utils/
        |  |
        |  └── ...
        |
        └── ...
```

---

## 3.11 arch Structure

The `arch/` directory contains all architecture-specific code

### Rules
- Each architecture gets its own Folder.
- There is a central API that uses preprocessors to ensure that only the correct architecture is compiled.
- `arch` may access non-architecture-specific components only through their APIs.

``` txt
    arch/
    |
    ├── i386/
    |   |
    |   |   # The structure is identical to that explained in "3.8 Kernel Structure".
    |   ├── kernel/
    |   |   |
    |   |   └── ...
    |   |
    |   |   # The structure is identical to that explained in "3.7 Runtime Structure".
    |   ├── runtime/
    |   |   |
    |   |   └── ...
    |   |
    |   └── ...
    |
    |   # Folders for other architectures, e.g., (x86_64, arm, misp)
    └── ...
```

---

# 4. Coding Conventions

---

## 4.1 General C++ Rules

- No exceptions (`throw`, `try`, and `catch` are prohibited)
- No RTTI (`dynamic_cast` and `typeid` are prohibited)
- No recursion
- No virtual methods
- No dynamic memory allocation, unless explicitly permitted by the subsystem
- No use of the C++ Standard Library
- No global variables except for intentionally defined singletons
- All functions must be `noexcept`, unless this is impossible
- All constructors and destructors must be `noexcept`
- All trivial constructors/destructors must be `= default`
- Anything that is possible and for which the compiler does not indicate that it has no effect should be declared as `const`
- Anything that is merely possible should be declared as `constexpr`
- Everything should be explicitly defined
- Each file contains one class as a module for encapsulation, which handles the main logic, except for helper files.
- No inline assembly (use `.asm` files instead)
- names should be written out in full; do not use abbreviations unless they are common and well-known.
- Use `[[likely]]` and `[[unlikely]]` in some places to optimize the hot path.
- `kernel_main` is declared as `[[noreturn]]`
- Use `[[nodiscard]]` for methods and functions where the return value cannot be ignored under any circumstances.
- Do not use C code in C++
- Declare C++ functions that serve as an assembly interface using `extern "C"`, `extern "C"`. 
This prevents name conflicts, which is why you can then use these functions even without namespaces.
- One class per file:
    - Helper files may contain multiple small helper functions, but no classes.
- No magic numbers:
    - All values that can become magic are declared in `const` variables.
    - All values that can become magic and are needed by multiple methods within a class are declared in `constexpr` variables in the `private:` section of the class.

---

## 4.2 C Code Rules

- No recursion
- No use of the C standard library
- No global variables
- C code should be avoided at all costs and used only when absolutely necessary, e.g., as a runtime API for the compiler
- Anything that is possible and for which the compiler does not specify that it has no impact should be declared as `const`
- names should be written out in full; do not use abbreviations unless they are common and well-known.
- Everything should be explicitly defined
- No inline assembler (use `.asm` files instead)
- C code should not be used as an assembly interface, unless there is no other option.
- No magic numbers:
    - All values that could become magic numbers are declared as `const` variables.

---

## 4.3 Assembler Rules

- No magic numbers—use macros
- Assembly code should be avoided at all costs and used only when there is no other option—for example, 
when maximum hardware control is required, or when a problem is easier to solve in assembly than in regular code.
- Labels and macro names should be written out in full; do not use abbreviations unless they are common and well-known.
- The assembler code must be architecture-specific and stored under `arch/[Architecture]/...`
- Do not use `%include`; use `extern` instead

---
