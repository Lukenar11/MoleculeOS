# MoleculeOS – Kernel Stack Initialization

The MoleculeOS kernel uses its own dedicated 32‑bit stack, 
separate from both the Real‑Mode stack and the temporary Stage‑2 Loader stack. <br>
This kernel stack is larger, aligned, and designed for long‑term use during normal kernel execution. <br>

---

## Purpose of the Kernel Stack

The kernel requires a dedicated stack because:

- the Real‑Mode stack is only 16‑bit and extremely small  
- the Stage‑2 Loader stack is temporary and only used during early boot  
- the kernel needs significantly more stack space for:  
  - function calls  
  - local variables  
  - interrupt handling  
  - exception frames  
  - context switching (later)

A stable, aligned, and sufficiently large stack is essential for reliable kernel execution.

---

## Kernel Stack Size and Alignment

MoleculeOS allocates a **512 KiB (0.5 MiB)** kernel stack.

Reasons for this size:

- plenty of space for deep call chains  
- safe for early debugging  
- avoids stack overflows during initialization  
- future‑proof for interrupt frames and exception handling  

The stack is **16‑byte aligned**, which is important for:

- ABI compatibility  
- SIMD instructions  
- potential future optimizations  

---

## Kernel Stack Code

The kernel stack is defined in the `.bss` section:

``` asm
    global kernel_stack_top
    align 16

    section .bss
        kernel_stack_bottom:
            ; Kernel-Stack (0.5 MiB)
            resb 512 * 1024

        kernel_stack_top:
```

This creates:

- a contiguous 512 KiB block  
- `kernel_stack_bottom` → lowest address  
- `kernel_stack_top` → highest address  

Because the x86 stack grows downward, the kernel sets:

```asm
    mov esp, kernel_stack_top
```

---

## Memory Layout of the Kernel Stack

<h6>
    <strong> Note: </strong> <br>
    This is a simplified excerpt of the MoleculeOS memory layout. <br>
    Only the regions relevant for kernel stack initialization are shown here.
</h6>

```text
    +---------------------------+  high memory
    | Kernel Stack (0.5 MiB)    |
    | grows downward            |
    +---------------------------+  kernel_stack_top
    |                           |
    | ...                       |
    |                           |
    +---------------------------+  kernel_stack_bottom
    | Kernel Code / Data        |
    | kernel.bin                |
    +---------------------------+  0x00010100
    | Stage 2 Loader (32-bit)   |
    +---------------------------+  0x00010000
    | Bootloader (Stage 1)      |
    +---------------------------+  0x00007C00
    | BIOS / IVT / BDA          |
    +---------------------------+  0x00000000
```

---

## Difference Between Loader Stack and Kernel Stack

| Stack Type        | Size        | Mode        | Purpose |
|-------------------|-------------|-------------|---------|
| Real‑Mode Stack   | 0.24 KiB    | 16‑bit      | Stage 1 bootloader |
| Loader Stack      | 0.24 KiB    | 32‑bit      | Stage 2 minimal loader |
| Kernel Stack      | 512 KiB     | 32‑bit      | Full kernel execution |

The kernel stack is the **first long‑term, stable stack** used by MoleculeOS.

---

## Summary

The kernel installs its own large, aligned **512 KiB** stack to replace the temporary stacks used during earlier boot stages. <br>
This stack is placed in the `.bss` section, grows downward, and becomes the primary execution stack for all kernel functions and (later) interrupt handling. <br> 
It ensures stability, safety, and enough space for complex kernel operations. <br>
