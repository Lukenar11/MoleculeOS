# **CHANGE LOG**

## *[0.1.0-alpha] – May 25, 2026*

### Added
- Fully functional 32-bit kernel  
- GDT initialization + Far Jump  
- IDT initialization + interrupt handlers
- Panic system with colored output  
- Reboot routine (`kernel::system::reboot`)  
- Shutdown routine (`kernel::system::shutdown`)  
- VGA text mode driver (cursor, colors, clear, reset)  
- PS/2 keyboard driver (scan codes, Shift, CapsLock, release events)  
- Runtime layer (`runtime/`)  
  - `text_output`  
  - `Array<T, N>`  
  - Utility functions  
  - Basic C runtime functions
- Shell system  
  - Input buffer  
  - Tokenizer  
  - Parser  
  - Compile-time command hashing  
  - Pipeline flush  
- Built-in shell commands  
  - `help`  
  - `info`  
  - `clear`  
  - `reboot`  
  - `shutdown`  
  - `echo`  
- Color-coded error handling in shell and interpreter  
- Consistent project structure (`kernel/`, `runtime/`, `drivers/`, `shell/`, ...)  
- STYLE.md (Chapters 1–3 complete)

### Changed
- Interpreter reworked (more stable tokenizer logic, overflow handling, colored errors)  
- Command system switched to constexpr hashing  
- Text output color scheme standardized  
- Project structure modularized  
- Runtime separated from hardware access (HAL principle)

### Fixed
- Scancode release handling  
- Null termination in command and argument buffers  
- Incorrect color reset after error messages  
- Minor off-by-one errors in the interpreter

### Known Issues
- No file system  
- No paging / memory manager  
- No multitasking  
- No scheduler
- No user mode
- No timer driver (PIT/APIC)
- Cursor may flicker in QEMU with <2 MB RAM (QEMU Slow Path)  
- No mouse support  
- No user programs
