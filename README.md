# MoleculeOS

**MoleculeOS** is a minimalist, text-based *i386* monolithic operating system, 
<br> developed in C++ as a hobby project.

The goal of this project is to create a clearly structured, <br>
modular learning and experimentation system that makes the fundamental building blocks of an operating system <br>
understandable and transparent - hence the name *MoleculeOS*.

---

## Features (current / planned)

- Minimal 32‑bit kernel in C++
- VGA text mode output
- Basic hardware initialization
- Interrupt Descriptor Table (IDT)
- Global Descriptor Table (GDT)
- Bump Allocator Heap
- PS/2 Keyboard input
- Shell Interpreter
- grub Boot-Setup
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

Below is the current state of MoleculeOS running in QEMU:

![MoleculeOS boot screenshot](docs/images/std-screen.png)

Below is the current state of MoleculeOS running in QEMU (deliberately triggered panic with a division by 0):

![MoleculeOS panic screenshot](docs/images/kernel-panic.png)

---

## Requirements

To build and run MoleculeOS, the following software is required:

- **Grub** (for the Boot setup and configurations)
- **NASM** (Netwide Assembler for assembeling the Bootloader and Assembly Fiels in the Kernel)
- **Clang++** (for compiling the 32‑bit kernel and the C++ runtime, requires i386 target support)
- **Clang** (for compiling the C Runtime and C komponentes in the Kernel, requires i386 target support)
- **Make** (for Building and running the Project)
- **QEMU** (for running the OS in a virtual machine)
- **WSL** (Windows Subsystem for Linux, if you work under Windows) (for using Grub and run Shell Skripts)

---

## Build & Run

## if you using VS Code
``` txt
    CTRL + SHIFT + B
```

## if you using the shell
``` bash
    chmod +x build.sh
    ./build.sh
```

This will:

- assemble the bootloader
- compile the kernel
- compile the runtime
- build a `.iso` File
- launch QEMU
