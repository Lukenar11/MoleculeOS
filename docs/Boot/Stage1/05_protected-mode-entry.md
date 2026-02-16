# Entering 32-Bit Protected Mode

This documentation describes the moment when MoleculeOS fully switches to 32-bit Protected Mode. <br>
At this point, the bootloader has:

- initialized the stack in real mode
- loaded the Global Descriptor Table **(GDT)**
- set the PE bit in `cr0`

---

## Requirements for Protected Mode
Before MoleculeOS can switch to Protected Mode, the following conditions must be met:

- GDT is loaded (`lgdt [GdtDescriptor]`)
- Code segment **(0x08)** and data segment **(0x10)** are correctly defined
- **PE (Protection Enable) bit** is set
- Interrupts are disabled (`cli`)
- Stack is set in real mode

Only then is the switch safe.

---

## Far Jump into the 32‑Bit Code Segment
After setting the PE bit, the CPU technically remains in Protected Mode, <br> but continues to interpret 16‑bit instructions as long as `cs` is not reloaded. <br> Therefore, a far jump is necessary

``` asm
jmp 0x08:ProtectedModeEntry
```
**0x08** is the segment selector for the 32‑bit code segment.

- The far jump reloads `cs`
- From this point, the CPU begins to interpret **32‑bit instructions**
- The **prefetch queue** is cleared
- The transition is fully completed

---

## Protected Mode Memory Layout (32‑Bit)

<h6>
    <strong> Note: </strong> <br>
    This is a simplified excerpt of the memory layout of MoleculeOS. <br>
    Only the areas relevant for the Protected Mode Entry are shown here.
</h6>

``` text
    +---------------------------+ 0x7C00
    | Bootloader (512 Bytes)    |
    | MoleculeOS Boot Sector    |
    +---------------------------+ 0x7E00
    | Global Descriptor Table   |
    | gdt, gdt_end, descriptor  |
    +---------------------------+ 0x7E20
    | Free / Stack Growth Area  |
    | ...                       |
    +---------------------------+
    | ↑↑↑                       |
    | Stack (grows downward)    |
    +---------------------------+ 0x100000 ← new 32‑Bit Stack (ESP)
    | Free / conventional RAM   |
    | ...                       |
    +---------------------------+
```

---

## Protected Mode Entry Code (32‑Bit)

``` asm
    [bits 32]

    ProtectedModeEntry:
        mov ax, 0x10    ; Data-Segment selector (GDT index 2)
        mov ds, ax      ; Data-Segment
        mov es, ax      ; Extra-Segment
        mov fs, ax      ; General-Purpose segment
        mov gs, ax      ; General-Purpose segment
        mov ss, ax      ; new Stack-Segment (32-bit)

        ; _start_ OsLoader
        jmp 0x08:0x7E00
```
---

## Why All Segment Registers Are Set

After the far jump, only `cs` is correctly set. <br> 
All other segments **(**`ds`**,** `es`**,** `fs`**,** `gs`**,** `ss`**)** still contain real-mode values, which are invalid in protected mode. 

Therefore, they must be immediately set to a valid data segment selector.

---

## Why the Stack is Being Reset

- The real-mode stack is 16-bit wide
- The protected-mode stack is 32-bit wide
- The old SS:SP is invalid in protected mode
- An incorrect stack immediately leads to a triple fault

Therefore:
```asm
    mov ss, ax
    mov esp, 0x100000
```

---

## Summary

The Protected Mode Entry:

- reloads all segment registers
- sets up a 32-bit stack
- ensures that the CPU is fully operating in Protected Mode
- forms the basis for the subsequent OS loader or kernel

This is the first real step towards a complete operating system.
