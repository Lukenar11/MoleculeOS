# MoleculeOS – Kernel Stack Memory Layout (0.5 MiB)

The kernel stack in MoleculeOS is allocated as a **contiguous 4-KiB block** in the `.bss` segment. <br>
It grows — like all x86 stacks — **downwards** and ends at the address `KernelStackTop`. <br>

---

## Memory Layout (visualized)

<h6>
    <strong> Note: </strong> <br>
    This is a simplified excerpt of the memory layout of MoleculeOS. <br>
    Only the areas relevant for the Protected Mode Entry are shown here.
</h6>

```text
    +-----------------------------+ 0x00007C00
    | Bootloader (Stage 1)        |
    | MoleculeOS Boot Sector      |
    | (512 Bytes)                 |
    +-----------------------------+ 0x00007E00
    | BIOS-safe area              |
    | (unused by MoleculeOS)      |
    +-----------------------------+ 0x00010000
    | Stage 2 Loader (32-bit)     |
    | Loader.asm                  |
    +-----------------------------+ 0x00010100
    | Kernel Entry Point          |
    | KernelEntry.asm             |
    +-----------------------------+ 0x00010200
    | Kernel .text section        |
    | executable kernel code      |
    +-----------------------------+
    | Kernel .rodata section      |
    | read-only data              |
    +-----------------------------+
    | Kernel .data section        |
    | initialized globals         |
    +-----------------------------+
    | Kernel .bss section         |
    | uninitialized globals       |
    | includes:                   |
    |   - KernelStackBottom       |
    |   - KernelStackTop          |
    +-----------------------------+ 0x00098000
    | KernelStackBottom           |
    | start of 0.5 MiB stack      |
    +-----------------------------+ 0x00098000
    |                             |
    |   Kernel Stack (0.5 MiB)    |
    |   grows downward            |
    |                             |
    +-----------------------------+ 0x000F8000
    | KernelStackTop              |
    | initial ESP in KernelEntry  |
    +-----------------------------+ 0x000B8000
```

---

# Kernel Stack Code (for reference)

```asm
    align 16

    section .bss

        ; Kernel-Stack (4 KiB)
        kernel_stack_bottom: 
            resb 4 * 1024

        KernelStackTop:
```

---

# Why This Layout Works Perfectly

### Stack is located in `.bss` → automatically zero-initialized
This is ideal for:

- deterministic behavior
- debugging
- clean register saves

### `KernelStackTop` is aligned
This is important for:

- performance
- interrupt frames
- possible SIMD extensions

### Stack is far away from kernel code
This avoids:

- overlaps
- memory corruption
- unwanted overwrites

---

# **Summary**

The MoleculeOS kernel stack is a **4‑KiB contiguous memory area** <br> 
that is reserved in the `.bss` segment and entirely controlled by the kernel. <br> 
It replaces the small temporary real-mode stack and provides the first stable runtime.
The stack grows — as is usual on x86 — **downwards** and starts at `KernelStackTop`, which is set as the new `esp`. <br> 
Because of its placement in the `.bss`, the entire stack is automatically **zero-initialized**, <br> 
which facilitates debugging and deterministic behavior. <br>
