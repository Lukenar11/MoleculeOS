# Initialization of the Stack (Real Mode) 

This section describes how MoleculeOS initializes the stack in 16-bit real mode during the boot procedure. <br>
The stack is one of the first structures that must be set up correctly before further steps such as loading the GDT or switching to protected mode can take place. 

--- 

## Goal of Stack Initialization 

The bootloader is loaded by the BIOS to the physical address **0x7C00**. <br>
For the kernel to operate reliably, it needs a **stable, safe stack** that: 

- does not grow into the bootloader 
- does not collide with BIOS structures 
- provides enough space for interrupts, function calls, and temporary data 

Therefore, MoleculeOS sets the stack pointer to **0x9C00**, a proven, safe memory area in conventional RAM.

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
    | Free / Stack Growth Area  |
    | ...                       |
    +---------------------------+ 0x8000
    | ↑↑↑                       |
    | Stack (grows downward)    |
    +---------------------------+ 0x9C00 ← Stackpointer (SP)
    | Free / conventional RAM   |
    | ...                       |
    +---------------------------+ 0xA000
```

---

The area **0x7C00–0x9C00** is free usable RAM and is not overwritten by the BIOS. <br>
It is therefore ideal for the stack.

---

## Why does the stack grow downward?

The x86_64 architecture defines the behavior of stack instructions:

- `push` → decreases `SP` by 2 and writes the value
- `pop` → reads the value and increases `SP` by 2

This means:
**The stack always grows towards lower addresses.**

Therefore, the stack pointer must be set to the **top** of the desired stack area.

---

## Code: Stack Initialization in MoleculeOS

```asm
    xor ax, ax      ; clear Accumulator
    mov ss, ax      ; set Stack-Segment
    mov sp, 0x9C00  ; set Stack-Size (8 KB)
```
