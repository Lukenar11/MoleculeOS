# MoleculeOS – STYLE.md (v1.0)
MoleculeOS Coding & Naming Style Guide
Valid for all kernel, driver, runtime, and for C, C++ & Assembly

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
