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
- `extern "C"` interfaces replaced C code where appropriate, and existing `extern "C"` declarations were wrapped in proper C++ namespaces.

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

---

## *[0.3.0‑alpha] – August 4, 2026*

**Added**
- Added `<circular_buffer.hpp>`
- ATA-PIO driver
- New terminal commands for filesystem interaction  
  - `allocinfo`
  - `copy`
  - `exists`
  - `rename`
- IRQ-handler for the keyboard driver `drivers::ps2::Keyboard_Input::keyboard_irq_handler();`
- Keyboard input buffer scancode_buffer `drivers::ps2::Keyboard_Input::scancode_buffer();`

### Changed
- Rewrite the allocator from a stack to a block allocator
- New Inode based flat custom file system with heap allocation and name hashing
- Specifying the file size with the `create` command
- Merging architecture-dependent and architecture-independent APIs
  - `<kernel_arch_api.hpp>` merged with `<kernel_api.hpp>`
  - `<io_arch_api.hpp>` merged with `<io_api.hpp>`
- Renaming libraries
  - `<string_manipulation.hpp>` renamed to `<string_manip.hpp>`
  - `<memory_manipulation.hpp>` renamed to `<memory_manip.hpp>`
- New Filesystem API
  - `kernel::filesystem::mofs.is_valid_file_name_or_formant_char();` → `kernel::filesys::MoleculeOS_File_System_2::is_valid_name_or_format_char();`
  - `kernel::filesystem::mofs.get_inode_by_name_and_format();` → `kernel::filesys::MoleculeOS_File_System_2::find_file();`
  - `kernel::filesystem::mofs.create_file();` → `kernel::filesys::MoleculeOS_File_System_2::create_file();`
  - `kernel::filesystem::mofs.delete_file();` → `kernel::filesys::MoleculeOS_File_System_2::delete_file();`
  - `kernel::filesystem::mofs.get_file_content_binary();` → [Removed]
  - `kernel::filesystem::mofs.get_file_content_as_string();` → [Removed]
  - `kernel::filesystem::mofs.read_file_binary_at();` → [Removed]
  - `kernel::filesystem::mofs.read_file_as_string_at();` → [Removed]
  - `kernel::filesystem::mofs.set_file_content_binary(); ` → [Removed]
  - `kernel::filesystem::mofs.set_file_content_as_string();` → [Removed]
  - `kernel::filesystem::mofs.write_file_binary_at();` → [Removed]
  - `kernel::filesystem::mofs.write_file_string_at();` → [Removed]
  - `kernel::filesystem::mofs.get_inodes();` → `kernel::filesys::MoleculeOS_File_System_2::get_inode_table();`
  - added `kernel::filesys::MoleculeOS_File_System_2::write_file();`
  - added `kernel::filesys::MoleculeOS_File_System_2::append_file();`
  - added `kernel::filesys::MoleculeOS_File_System_2::clear_file();`
  - added `kernel::filesys::MoleculeOS_File_System_2::rename_file();`
  - added `kernel::filesys::MoleculeOS_File_System_2::copy_file();`
  - added `kernel::filesys::MoleculeOS_File_System_2::read_file();`
  - added `kernel::filesys::MoleculeOS_File_System_2::resize_file_size();`
- New heap allocator API
  - `kernel::heap::stack.allocate();` → `kernel::heap::Block_Allocator::allocate();`
  - `kernel::heap::stack.rewind();` → `kernel::heap::Block_Allocator::deallocate();`
  - `kernel::heap::stack.mark();` → [Removed]
  - `kernel::heap::stack.used();` → [Removed]
  - `kernel::heap::stack.remaining();` → [Removed]
  - added `kernel::heap::Block_Allocator::reallocate();`
  - added `kernel::heap::Block_Allocator::get_allocation_info();`
- New heap size (**64KB** → **2MB**)
- Moved most of the stack initialization from the boot to the linker script.
- New filename for `<stdint.h>` → `<types.h>`
- Removed `intpure_t` & `uintptr_t`
- Connecting the keyboard driver to the IRQ
- Project folder structure
- rename `system/` → `sys/`
- rename `filesystem/` → `filesys/`
- rename `commands/` → `cmds/`
- rename `commands.hpp` → `cmds.hpp`
- rename `mofs.hpp/cpp` → `mofs2.hpp/cpp`
- rename `stack.hpp/cpp` → `block.hpp/cpp`
- rename `trigger_interrupt.asm` → `interrupts.asm`
- rename `cpu_flags.asm` → `eflags.asm`
- rename `hang.asm` → `halt.asm`
- rename `init_kernel.hpp/cpp` → `kernel_arch_init.hpp/cpp`
- rename `filesystem::` → `filesys::`
- rename `IDT` → `Interrupt_Descriptor_Table/`
- rename `MoleculeOS_File_System` → `MoleculeOS_File_System_2`
- rename `Stack_Allocator` → `Block_Allocator`
- move `kernel_stack.asm` → `linker.ld` 
- move `gdt.asm` → `_start.asm` 
- Delete all global objects using static classes.
- Updated STYLES.md for the new folder structure.
- Makefile structure

### Improved
- New file system, with more flexibility and better usage
- New heap allocator with more flexibility
- More memory for the Heap
- New simplified API structure
- Moved most of the stack initialization from the boot script to the linker script.
- Stack and heap initialization are in the same file.
- No unnecessary type aliases
- Better buffering
- Interrupt based keyboard driver
- Better Project folder structure
- No global objects
- Better Makefile structure
- faster build time

### Fixed
- Outdated references in STYLES.md 

### Known Issues
- No multitasking  
- No scheduler  
- No user mode  
- No paging / memory manager  
- No mouse support  
- No user programs  
- Filesystem is volatile
- Filesystem is flat
