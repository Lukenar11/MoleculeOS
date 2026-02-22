# MoleculeOS – Stage 2 Loader (Loader.asm)

The Stage 2 Loader is the first piece of MoleculeOS that runs in **32‑bit Protected Mode**. <br>
It is intentionally minimal and performs only the essential steps required before the kernel can take over. <br>

Stage 1 (the Real‑Mode bootloader) performs:

- reset the Boot Stack
- loading the kernel into memory   

Once Stage 2 begins, MoleculeOS is executing pure 32‑bit code.

---

## Purpose of the Stage 2 Loader

The Loader performs only three tasks:

- disable interrupts (safety during early boot)
- set a small temporary 32‑bit stack (256 bytes)
- call the kernel entry point (`KernelEntry`)

The Loader does **not**:

- initialize C++ runtime  
- set up paging  
- load drivers  
- parse ELF files  
- install IDT or GDT  

All of this is handled later by the kernel itself.

---

## Why the Loader Is Minimal

MoleculeOS follows a clean and modern boot architecture:

``` text
    Stage 1 (Real Mode) → Stage 2 (32-bit Loader) → KernelEntry → main()
```

Stage 2 is intentionally kept small because:

- the kernel has its own stack  
- the kernel performs all CPU‑level initialization  
- the Loader should not duplicate work  
- a minimal Loader is easier to debug and maintain  

This design mirrors the structure of many modern OS projects.

---

## Memory Layout at the Start of Stage 2

Stage 1 loads the Loader to physical address **0x00010000** and jumps to it after enabling Protected Mode.

<h6>
    <strong> Note: </strong> <br>
    This is a simplified excerpt of the MoleculeOS memory layout. <br>
    Only the regions relevant for the OS Loader are shown here.
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
    | Free / conventional RAM   |
    | ...                       |
    +---------------------------+ 0x10000
    | Stage 2 Loader (32-bit)   |
    | Loader.asm (entry: Loader)|
    +---------------------------+ 0x10100
    | Kernel Entry Point        |
    | KernelEntry.asm           |
    +---------------------------+ ...
```

---

## Why the Stack Must Be Reset

After switching to Protected Mode:

- the Real‑Mode stack is invalid  
- `ss`:`sp` still contains 16‑bit values  
- any `push` or `call` would cause undefined behavior  

Therefore, the Loader sets a **small temporary 32‑bit stack**:

```asm
    ; reset Stack (0.25 KiB)
    mov esp, 0x00FF
```

This stack is only used for:

- the `call KernelEntry` instruction  
- a few pushes/pops during early kernel startup  

The kernel installs its own large stack immediately afterward.

---

## Full Loader Code

```asm
    [bits 32]

    global Loader 

    ; Kernel/Boot/KernelEntry.asm
    extern KernelEntry

    section .text
        Loader:
            cli                 ; Interrupts off
            mov esp, 0x00FF     ; reset Stack (0.25 KiB)
            call KernelEntry    ; _init_ Kernel
```

---

## Execution Flow of the Loader

``` text
    +-----------------------------+
    | Stage 1 Bootloader (16-bit) |
    +-------------+---------------+
                  |
                  v
    +-----------------------------+
    | Protected Mode enabled      |
    | GDT loaded                  |
    +-------------+---------------+
                  |
                  v
    +-----------------------------+
    | Far jump → Loader (32-bit)  |
    +-------------+---------------+
                  |
                  v
    +-----------------------------+
    | Loader                      |
    |   - disable interrupts      |
    |   - set temporary stack     |
    |   - call KernelEntry        |
    +-------------+---------------+
                  |
                  v
    +-----------------------------+
    | MoleculeOS Kernel           |
    +-----------------------------+
```

---

## Summary

The Stage 2 Loader is the first fully 32‑bit component of MoleculeOS. <br>
It is intentionally minimal and exists only to provide a clean and safe transition from the Real‑Mode bootloader into the protected‑mode kernel.

It performs exactly three tasks:

- interrupts are disabled to avoid undefined behavior during early boot
- a temporary 256‑byte 32‑bit stack is installed
- control is transferred to the kernel’s entry point (KernelEntry)

The Loader does not initialize hardware, C++ runtime, or system structures. <br>
All advanced initialization is handled by the kernel itself. <br>

This minimalistic design keeps the boot pipeline simple, predictable, <br>
and easy to debug — ensuring that MoleculeOS enters its kernel in a clean and well‑defined state.
