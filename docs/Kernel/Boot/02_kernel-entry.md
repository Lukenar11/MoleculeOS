# MoleculeOS – Kernel Entry

The Kernel Entry is the first function executed inside the MoleculeOS kernel after the Stage 2 Loader transfers control. <br>
It runs in **32‑bit Protected Mode** and is responsible for preparing the CPU environment before the kernel’s main logic begins. <br>

KernelEntry is intentionally minimal and focuses only on essential CPU setup.

---

## Purpose of KernelEntry

KernelEntry performs the following tasks:

- install the dedicated **kernel stack**
- ensure segment registers reference valid 32‑bit descriptors
- prepare the CPU state for high‑level kernel code
- transfer control to the kernel’s main entry point

KernelEntry does **not**:

- load the GDT (done in Stage 1)
- switch to Protected Mode (done in Stage 1)
- initialize hardware
- install the IDT

All advanced initialization is performed later by the kernel itself.

---

## Requirements Before KernelEntry Runs

When the CPU jumps into KernelEntry, the following conditions are already guaranteed:

- Protected Mode is active
- the GDT is loaded and valid
- the code segment (CS) is a 32‑bit segment
- the Loader has set a temporary 32‑bit stack
- interrupts are disabled

This ensures KernelEntry can safely perform its setup without relying on BIOS services or Real‑Mode structures.

---

## Kernel Stack Activation

KernelEntry switches from the Loader’s temporary stack to the kernel’s dedicated 512 KiB stack:

```asm
    mov esp, kernel_stack_top   ; set Kernel-Stack Size (0.5 MiB)
```

This marks the moment where the kernel becomes fully independent from the bootloader.

---

## Why `ebp` Is Set After the Stack

Immediately after setting `esp`, KernelEntry also initializes the **frame pointer**:

```asm
    mov ebp, esp    ; Stack-Frame Pointer
```

`ebp` (Base Pointer) serves as a **stable reference point** for the current stack frame. <br>
While `esp` constantly changes during `push`, `pop`, interrupts, and function calls, `ebp` remains fixed throughout a function. <br>

This provides:

- reliable access to function arguments  
- reliable access to local variables  
- clean stack traces  
- proper debugging support  
- compatibility with C/C++ calling conventions  

Setting `ebp = esp` creates the **first clean stack frame** of the kernel, ensuring that all later functions operate on a well‑defined foundation.

---

## Memory Layout Around KernelEntry

<h6>
    <strong> Note: </strong> <br>
    This is a simplified excerpt of the MoleculeOS memory layout. <br>
    Only the regions relevant for kernel entry are shown here.
</h6>

```text
    +---------------------------+  high memory
    | Kernel Stack (0.5 MiB)    |
    | grows downward            |
    +---------------------------+  kernel_stack_top
    | Kernel Code / Data        |
    | kernel.bin                |
    +---------------------------+  KernelEntry
    | Stage 2 Loader (32-bit)   |
    +---------------------------+  0x00010000
    | Bootloader (Stage 1)      |
    +---------------------------+  0x00007C00
```

KernelEntry sits at the beginning of the kernel image and is the first executed instruction inside the kernel.

---

## Example Implementation (simplified)

``` asm
    global KernelEntry

    extern kernel_stack_top
    extern main

    section .text
        KernelEntry:
            ; _init_ Kernel-Stack (0.5 MiB)
            mov esp, kernel_stack_top   ; set Kernel-Stack Size (0.5 MiB)
            mov ebp, esp                ; set Stack-Frame Pointer

            ; _start_ C++-Kernel (kernel::main())
            call main
```

This is intentionally minimal and easy to understand.

---

## Summary

KernelEntry is the first function executed inside the MoleculeOS kernel. <br> 
It installs the kernel’s dedicated stack, initializes the frame pointer for clean stack frames, and transfers control to the kernel’s main function. <br>
All heavy initialization (IDT, drivers, memory manager) happens later, keeping KernelEntry simple, predictable, and easy to debug. <br>
