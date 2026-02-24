# MoleculeOS

**MoleculeOS** is a minimalist, text-based monolithic operating system, <br>
developed in C++ by GitHub user *Lukenar11 (Luke Matthes)*.

The goal of this project is to create a clearly structured, <br>
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

## Current Boot Output

Below is the current state of MoleculeOS running in QEMU (VGA-Driver-Test):

![MoleculeOS boot screenshot](docs/images/qemu-vga-driver-test.png)

---

## Documentation

All technical documentation is located in the [docs/](docs/) directory.

A complete overview of the OS components can be found in:

- [Boot Process](docs/Boot/00_boot.md)
- [Kernel](docs/Kernel)
- [Runtime](docs/Atom/00_atom.md)
- [Terminal](docs/Terminal/Drivers/VGA/01_vga-driver.md)

Subsystem documentation is organized into:

- [Real-Mode bootloader](docs/Boot/Stage1)
- [Protected-Mode OS loader](docs/Boot/Stage2)
- [Kernel Boot](docs/Kernel/Boot)
- [Kernel IDT](docs/Kernel/IDT)
- [Ternimal VGA-Driver](docs/Terminal/Drivers/VGA/01_vga-driver.md)

---

## Requirements

To build and run MoleculeOS, the following software is required:

- **Node.js** (used as the build orchestration / glue layer)
- **NASM** (Netwide Assembler for Stage‑1, Stage‑2 and Assembly Fiels in the Kernel)
- **Clang++** (for compiling the 32‑bit kernel, requires i386 target support)
- **Clang** (for compiling the C Runtime, requires i386 target support)
- **QEMU** (for running the OS in a virtual machine)

All build scripts are located in the `build/` directory and are executed via Node.js.

---

## Build & Run

``` bash
    node build/build.js
```

This will:

- assemble the bootloader
- assemble the OS loader
- compile the kernel
- compile the runtime
- build a floppy disk image
- launch QEMU

---

## Build System & Glue Language

MoleculeOS uses **Node.js** as a lightweight “glue layer” to orchestrate the
entire build pipeline.  
All build scripts are located in the `build/` directory.

Node.js is not part of the operating system itself — it simply automates:

- assembling the Stage‑1 bootloader  
- assembling the Stage‑2 OS loader  
- compiling the C++ kernel  
- generating the disk image  
- launching QEMU  

This keeps the build process modular, fast, and easy to extend.
