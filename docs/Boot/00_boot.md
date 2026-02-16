# MoleculeOS – Complete Boot Process Overview

A full overview of the 16‑bit and 32‑bit memory layout, <br>
the entire boot procedure, and references to all subsystem documentation.

---

## Introduction

MoleculeOS uses a classic two‑stage boot process:

``` text
    Stage 1 (Bootloader, 16‑bit Real Mode)
    → Protected Mode transition
    → Stage 2 (OsLoader, 32‑bit)
    → Kernel (C++)
```

This document provides:

- the full 16‑bit memory layout
- the full 32‑bit memory layout
- the complete boot procedure
- references to all detailed documentation files

It serves as the master overview of the MoleculeOS boot architecture.

---

## Full 16‑Bit Memory Layout (Real Mode)

This layout shows the system state before switching to Protected Mode.

``` text
    +---------------------------+ 0x0000
    | Interrupt Vector Table    |
    | (IVT, 1 KB)               |
    +---------------------------+ 0x0400
    | BIOS Data Area (BDA)      |
    | (256 Bytes)               |
    +---------------------------+ 0x0500
    | BIOS Workspace / RAM      |
    | (keyboard buffer, etc.)   |
    +---------------------------+ 0x7C00
    | Bootloader (512 Bytes)    |
    | MoleculeOS Boot Sector    |
    +---------------------------+ 0x7E00
    | Global Descriptor Table   |
    | gdt, gdt_end, descriptor  |
    +---------------------------+ 0x7E20
    | Free / conventional RAM   |
    | ...                       |
    +---------------------------+ 0x9C00 ← Stack Pointer (SP)
    | ↑↑↑                       |
    | Stack (grows downward)    |
    +---------------------------+ 0xA000
    | VGA Text Mode Memory      |
    | (0xB8000 physical)        |
    +---------------------------+ 0xFFFF
    | BIOS ROM / Option ROMs    |
    +---------------------------+
```

---

## Full 32‑Bit Memory Layout (Protected Mode)

After the far jump, MoleculeOS runs in full 32‑bit mode.

``` text
    +---------------------------+ 0x00000000
    | Null Page (unused)        |
    +---------------------------+ 0x00001000
    | GDT / PM Structures       |
    | (loaded by Stage 1)       |
    +---------------------------+ 0x00007C00
    | Bootloader (Stage 1)      |
    | (no longer used)          |
    +---------------------------+ 0x00010000
    | OsLoader (Stage 2)        |
    | Entry Point               |
    +---------------------------+ 0x00011000
    | Kernel Code / Data        |
    | kernel.bin                |
    +---------------------------+ 0x0009FC00 ← Stack Pointer (ESP)
    | ↑↑↑                       |
    | Temporary 32‑bit Stack    |
    | (~0.5 KiB)                |
    | (grows downward)          |
    +---------------------------+ 0x000A0000
    | Free / conventional RAM   |
    | ...                       |
    +---------------------------+ 0x000B8000
    | VGA Text Mode Memory      |
    | (0xB8000 physical)        |
    +---------------------------+ 0x000FFFFF
    | Reserved / ROM Shadow     |
    +---------------------------+

```

---

## Complete Boot Procedure

This is the full, step‑by‑step boot sequence of MoleculeOS.

---

Stage 1 – Bootloader (Real Mode)
Responsibilities:

- loaded by BIOS at **0x7C00**
- sets up segment registers
- sets up a temporary Real‑Mode stack
- loads Stage 2 from disk using `int 13h`
- prepares the GDT
- switches to Protected Mode

---

# full Boot-Loader Code (Stage 1)

``` asm
    [org 0x7C00]
    [bits 16]

    jmp BootProcedure

    BootProcedure:
        ; Interrupts off
        cli

        ; _init_ Stack
        xor ax, ax      ; clear Accumulator
        mov ss, ax      ; set Stack-Segment
        mov sp, 0x9C00  ; set Stack-Size (8 KB)

        ; _init_ Disk-Loader
        call DiskReadLoader

        ; _init_ GDT
        lgdt [GdtDescriptor]

        ; _init_ Protected-Mode
        mov eax, cr0    ; Control => Accumulator (32 Bit)
        or eax, 1       ; set Protection Enable
        mov cr0, eax    ; Protection Enable => Control

        ; _start_ Protected-Mode
        call 0x08:ProtectedModeEntry

    %include "Boot/Stage1/Gdt.asm"
    %include "Boot/Stage1/ProtectedModeEntry.asm"
    %include "Boot/Stage1/DiskReadLoader.asm"

    times 510-($-$$) db 0
    dw 0xAA55
```
---

## Documentation References

This master document links to all detailed MoleculeOS documentation:

- [01 – Stack initialisation](Stage1/01_stack.md)
- [02 – Disk read loader](Stage1/02_disk-read-loader.md)
- [03 – GDT initialisation](Stage1/03_gdt.md)
- [04 – Protected-Mode initialisation](Stage1/04_protected-mode.md)
- [05 – Protected-Mode entry](Stage1/05_protected-mode-entry.md)
- [01 – OS Loader](Stage2/01_os-loader.md)

---

## Summary

This document provides a complete overview of the MoleculeOS boot architecture, <br>
covering the entire transition from 16‑bit Real Mode to 32‑bit Protected Mode. <br>
It includes the full memory layout for both execution modes, the complete <br>
Stage‑1 bootloader code, and references to all subsystem documentation. <br>

Together, these components form the foundation of the MoleculeOS boot pipeline:
- from BIOS → Bootloader → Protected-Mode → OsLoader → C++ Kernel.
