# MoleculeOS

**MoleculeOS** is a minimalist, text-based monolithic operating system, <br>
developed in C++ by GitHub user *Lukenar11 (Luke Matthes)*.

The goal of this project is to create a clearly structured, 
modular learning and experimentation system that makes the fundamental building blocks of an operating system <br>
understandable and transparent - hence the name *MoleculeOS*.

---

## Features (current / planned)

- Custom BIOS boot setup (x86)
- Stage‑1 boot sector + Stage‑2 loader
- Minimal 32‑bit kernel in C++
- VGA text mode output
- Basic hardware initialization
- Interrupt Descriptor Table (IDT)
- More subsystems to follow

---

## Design Philosophy

MoleculeOS is intentionally simple:

- no paging  
- no multitasking  
- no 64‑bit mode  
- no complex drivers  

The goal is to understand the *essence* of OS development without unnecessary complexity.

---

## Documentation

Detailed technical documentation can be found in the `docs/` folder.

---

## Build & Run

```bash
    node build/build.js
```

This will:

- assemble the bootloader
- assemble the OS loader
- compile the kernel
- build a floppy disk image
- launch QEMU
