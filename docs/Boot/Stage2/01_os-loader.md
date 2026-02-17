# MoleculeOS – Stage 2 Loader (OsLoader)

The OsLoader is the second stage of the MoleculeOS boot process.

It is loaded and executed by Stage 1 after:
- the kernel image has been copied into memory
- the Global Descriptor Table **(GDT)** has been loaded
- the CPU has switched into **32‑bit Protected Mode**

The OsLoader is intentionally minimal:
It sets up a valid 32‑bit stack, prepares the environment for C++ execution, <br> 
calls the kernel entry point, and halts if the kernel ever returns.

---

## Purpose of the OsLoader

The OsLoader performs the following tasks:
- initialize a new valid 32‑bit stack
- call the kernel entry function (`kernel::main`)
- ensure a stable transition into the kernel
- remain in a halt loop if the kernel returns unexpectedly

The OsLoader is the first fully 32‑bit component of MoleculeOS.

---

## Why a Stage‑2 Loader Is Necessary

The Stage‑1 bootloader is:
- 16‑bit Real Mode
- limited to 512 bytes
- dependent on BIOS interrupts
- unable to execute C++ code
- unable to run a Protected‑Mode kernel

The OsLoader acts as the bridge between:

``` text
    BIOS → Real-Mode → Protected-Mode → C++ Kernel
```
---

## Memory Layout at the Start of Stage 2

Stage 1 loads the OsLoader to the physical address `0x00010000`.

The kernel linker script is aligned to this address.

---

## Mode Memory Layout (32‑Bit)

<h6>
    <strong> Note: </strong> <br>
    This is a simplified excerpt of the memory layout of MoleculeOS. <br>
    Only the areas relevant for the Stage 2 Loader are shown here.
</h6>

``` text
    +---------------------------+ 0x07C00
    | Bootloader (Stage 1)      |
    | MoleculeOS Boot Sector    |
    +---------------------------+ 0x07E00
    | Stage 2 Load Buffer       |
    | (temporary load area)     |
    +---------------------------+ 0x10000
    | OsLoader Entry Point      |
    | Stage 2 (32-bit)          |
    +---------------------------+ 0x11000
    | Kernel Code / Data        |
    | kernel.bin                |
    +---------------------------+ ...
```

Stage 2 is placed directly before the kernel and executed immediately after the Protected‑Mode transition.

---

## Transition from Stage 1 to Stage 2 (Far Jump)

After enabling Protected Mode (setting the **PE** bit in `cr0`), the CPU is technically in 32‑bit mode, <br>
but still executing instructions using the old Real‑Mode cs selector.

To fully enter 32‑bit execution, Stage 1 performs a far jump into the OsLoader:

``` asm
    ; _start_ OsLoader (Stage 2) (fully 32-Bit Mode)
    jmp 0x08:0x7E00
```

Where:

- **0x08** → 32‑bit code segment selector from the GDT
- **0x10000** → physical address of the **OsLoader**

This far jump:
- reloads `cs`
- flushes the **prefetch queue**
- activates **32‑bit instruction decoding**
- transfers control to **Stage 2**

From this point on, MoleculeOS is executing pure 32‑bit code.

---

## Why the Stack Must Be Reset

After switching to Protected Mode, the old Real‑Mode stack is invalid:

- Real‑Mode stack is 16‑bit
- Protected‑Mode stack is 32‑bit
- **SS:SP** still contains Real‑Mode values
- Any call or push would cause a triple fault

Therefore, the OsLoader sets a temporary 32‑bit stack:

``` asm
    ; new Stack-Size (~0.5 MiB)
    mov esp, 0x83F00
```

This region is:
- free
- below 1 MiB
- safe for early kernel operations

The kernel later installs its own stack.

---

## Transition to C++ (kernel::main)

The OsLoader calls the kernel entry point:

``` asm
    ; _start_ Kernel
    call main
```
The function must be declared in C++ as:

``` cpp
    namespace kernel {

        extern "C" void main() {

            /* ... */
        }
    } // namespace kernel
```

This prevents C++ name mangling and ensures the symbol is callable from assembly.

---

## Full OsLoader Code

``` asm
    [bits 32]

    global OsLoader 

    ; kernel::main (C++ Function)
    extern main

    section .text
        OsLoader:
            ; new Stack-Size (~0.5 MiB)
            mov esp, 0x83F00

            ; _start_ Kernel
            call main

            .hang:
                hlt
                jmp .hang

```

---

## Execution Flow of the OsLoader

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
    | Far jump → OsLoader         |
    +-------------+---------------+
                  |
                  v
    +-----------------------------+
    | OsLoader (32-bit)           |
    |   - Set stack               |
    |   - Call kernel::main()     |
    +-------------+---------------+
                  |
                  v
    +-----------------------------+
    | MoleculeOS Kernel (C++)     |
    +-----------------------------+
```

---

## Summary

The OsLoader is a minimal but essential component of the MoleculeOS boot process.
It:

- sets a valid 32‑bit stack
- calls the kernel entry point
- ensures a stable transition
- halts if the kernel returns

Its simplicity reduces complexity, improves debugging, and ensures a clean handoff to the kernel.
