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

---

## *[0.2.0‑alpha] – June 28, 2026*

### Added
- flat Filesystem subsystem (creation, deletion, listing, inode handling)  
- New terminal commands for filesystem interaction  
  - `create`
  - `remove`
  - `list`
  - updated `echo` with simple file and text stream support
- Architecture folder (`arch/`) with clean separation between architecture‑dependent and architecture‑independent code  
- Central APIs for all components  
  - `drivers_api.hpp`
  - `kernel_api.hpp`
  - `shell_api.hpp`
  - `terminal_api.hpp`
- Architecture‑specific APIs using preprocessor‑controlled dispatch
  - `io_arch_api.hpp`
  - `kernel_arch_api.hpp`
- STYLE.md Chapter 4: Coding Conventions  
- STYLE.md additions for architecture structure  
- Updated STYLE.md runtime structure section  
- Hot‑path optimization rules across the entire codebase  
- New `stack_allocator` (renamed from `linear_area`)  
- New C++ namespace‑based linkage for all assembly interfaces and `extern "C"` deklarations

### Changed
- C code removed from the system (except minimal compiler ABI layer)  
- C runtime fully removed, except for `stdint.h`
- Runtime structure reworked and splittet into a C++ runtime and APIs
- All components now communicate strictly through their APIs  
- Assembly interfaces rewritten into C++ with proper namespaces  
- Port I/O functions renamed
  - `inb` → `byte_input`  
  - `inw` → `word_input`  
  - `outb` → `byte_output`  
  - `outw` → `word_output`  
- Replace the `string.h` library with the custom C++ libraries
  - `string_manipulation.hpp`
  - `memory_manipulation.hpp`
- Replace the function from `string.h` with custom C++ functions
  - `memcpy` → `runtime::memory_manip.copy_memory_block`
  - `memmove` → `runtime::memory_manip.move_memory_block`
  - `memset` → `runtime::memory_manip.set_memory_block`
  - `memcmp` → `runtime::memory_manip.compare_memory_block`
  - `strncpy` → `runtime::string_manip.copy_string_part`
  - `strcpy` → `runtime::string_manip.copy_string`
  - `strlen` → `runtime::string_manip.get_string_length`
  - `strcmp` → `runtime::string_manip.compare_strings`
  - `strchr` → `runtime::string_manip.find_char_in_string`
- Terminal command system updated  
- STYLE.md Chapters 1–3 updated to reflect new architecture  
- Project structure improved for portability and clarity  

### Improved
- Clear separation between architecture‑dependent and architecture‑independent components  
- Improved portability through strict arch folder structure  
- Improved maintainability via API‑only communication  
- Hot‑path performance improvements in all OS components 
- Consistency across naming conventions and file structures  

### Fixed
- Terminal bug where clearing near the bottom of the screen caused missing output  
- Incorrect naming of the old bump allocator  
- Outdated references in STYLE.md  

### Known Issues
- No multitasking  
- No scheduler  
- No user mode  
- No paging / memory manager  
- No mouse support  
- No user programs  
- Filesystem not yet journaled or crash‑safe  
- Filesystem is volatile
- Filesystem is flat
