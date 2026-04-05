# MoleculeOS – Kernel Entry (KernelEntry.asm)

The Kernel Entry is the **first code** that MoleculeOS executes after the Stage‑2 Loader has reset the temporary stack and called `KernelEntry`.<br> 
It forms the bridge between the minimal loader and the actual kernel main function (`kernel_main()`).<br>

KernelEntry only performs two tasks:

- install the **kernel stack**
- call the **kernel main function** (`kernel_main`)

All further initializations (runtime, IDT, drivers, memory management) take place in the kernel itself.

---

## Purpose of KernelEntry

KernelEntry is intentionally kept minimal and serves exclusively to:

- activate the **real kernel stack**
- set a valid **stack frame pointer**
- call the kernel main function

KernelEntry **does not initialize hardware**, <br>
**does not load drivers**, and **does not configure CPU structures**. <br>
These tasks belong in the kernel and not in the entry stub. <br>

---

## Why KernelEntry Is Minimal

MoleculeOS follows a clear boot architecture:

```text
    Stage 1 (Real Mode) → Stage 2 Loader (32-bit) → KernelEntry → kernel_main()
```

KernelEntry remains minimal because:

- the loader has already enabled Protected Mode
- the kernel needs its own stack size
- the kernel itself initializes all subsystems
- a small entry stub is easier to debug

---

## Memory Layout Around KernelEntry

<h6>
    <strong> Note: </strong> <br>
    This is a simplified excerpt of the memory layout of MoleculeOS. <br>
    Only the areas relevant for the Protected Mode Entry are shown here.
</h6>

```text
    +-----------------------------+ 0x00010000
    | Stage 2 Loader (32-bit)     |
    +-----------------------------+ 0x00010100
    | Kernel Entry Point          |
    | KernelEntry.asm             |
    +-----------------------------+ 0x00010200
    | Kernel .text section        |
    | executable kernel code      |
    +-----------------------------+
    | Kernel .data / .bss         |
    | includes KernelStackBottom  |
    | and KernelStackTop          |
    +-----------------------------+
```

---

## Why the Stack Must Be Installed Here

The loader's temporary stack is:

- very small (0.25 KiB)
- intended only for the transition
- not suitable for interrupts
- not designed for deeper function chains

The kernel requires its own stable stack:

- 512 KiB
- in the `.bss` segment
- zero-initialized
- aligned
- safe for interrupt frames

Therefore, KernelEntry sets the new stack **immediately**.

---

## Full KernelEntry Code

```asm
    global KernelEntry

    extern KernelStackTop
    extern kernel_main

    section .text

        KernelEntry:
            ; _init_ Kernel-Stack
            mov esp, KernelStackTop ; install Kernel Stack
            mov ebp, esp            ; initialize base pointer

            ; _start_ Kernel
            call kernel_main
```

---

## Execution Flow of KernelEntry

```text
    +-----------------------------+
    | Stage 2 Loader              |
    | (temporary 32-bit stack)    |
    +-------------+---------------+
                  |
                  v
    +-----------------------------+
    | KernelEntry                 |
    |   - install Kernel Stack    |
    |   - set EBP                 |
    |   - call kernel_main        |
    +-------------+---------------+
                  |
                  v
    +-----------------------------+
    | kernel_main()               |
    |   - runtime init            |
    |   - IDT/ISR setup           |
    |   - drivers init            |
    |   - memory mgmt init        |
    +-----------------------------+
```

---

# Summary

KernelEntry is the first real entry point into the MoleculeOS kernel. <br>
It exclusively takes on the tasks that are necessary to provide the kernel with a stable runtime environment:  

- the kernel stack is installed  
- the stack frame pointer is set  
- the kernel main function (`kernel_main`) is called  

KernelEntry is deliberately kept minimal in order to keep the boot process clear, stable, and easy to follow. <br>
All complex initializations only take place within the kernel itself. <br>
