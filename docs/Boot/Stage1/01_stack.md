# Initialization of the Stack (Real Mode) 

This section describes how MoleculeOS initializes the stack in 16-bit real mode during the boot procedure. <br>
The stack is one of the first structures that must be set up correctly before further steps such as loading the GDT or switching to protected mode can take place. 

--- 

## Goal of Stack Initialization 

The BIOS loads the MoleculeOS boot sector to **0x7C00**. <br>
The bootloader itself is extremely small and performs only a few instructions before switching to the OS loader.

--- 

Because of this minimal design, MoleculeOS uses a very small temporary stack during Stage 1:

- it must not overwrite the bootloader
- it must not collide with BIOS data
- it must be large enough for a few `call/push` instructions

A **256‑byte stack** is more than sufficient for this purpose.

---

## Memory Layout During the Boot Process

<h6>
    <strong> Note: </strong> <br>
    This is a simplified excerpt of the MoleculeOS memory layout. <br>
    Only the regions relevant for stack initialization are shown here.
</h6>

``` text
    +---------------------------+ 0x7C00
    | Bootloader (512 Bytes)    |
    | MoleculeOS Boot Sector    |
    +---------------------------+ 0x7E00
    | BIOS-safe area            |
    | (unused by MoleculeOS)    |
    +---------------------------+ 0x0000
    | Temporary Stack (0.25 KiB)|
    | grows downward            |
    +---------------------------+ 0x00FF ← Stack Pointer (SP)
```

---

The stack is placed in the low conventional memory region, <br>
which is unused by the BIOS and safe for temporary bootloader operations.

---

## Why does the stack grow downward?

The x86 architecture defines the behavior of stack instructions:

- `push` → decreases `sp` by 2 and writes the value
- `pop` → reads the value and increases `sp` by 2

This means:
**The stack always grows towards lower addresses.**

Therefore, the stack pointer must be set to the **top** of the desired stack area.

---

## Code: Stack Initialization in MoleculeOS

``` asm
    xor ax, ax      ; clear Accumulator
    mov ss, ax      ; set Stack-Segment
    mov sp, 0x00FF  ; set Stack-Size (0.25 KiB)
```

---

## Summary

MoleculeOS uses a tiny 256‑byte Real‑Mode stack placed safely in low memory. <br>
It sits below all BIOS and bootloader data and grows downward, as required by the x86 architecture. <br>
This stack is only temporary and is replaced later when the system switches to 32‑bit mode. <br>
