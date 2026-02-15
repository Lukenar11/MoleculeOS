# Initialization of Protected Mode

This section describes how MoleculeOS switches from 16‑bit real mode into 32‑bit protected mode.<br>
This step requires a correctly initialized stack and a valid Global Descriptor Table (GDT).

---

## Requirements before entering protected mode

Before the CPU can safely enter protected mode, MoleculeOS ensures:

- a valid stack is set up in real mode
- a GDT is defined and loaded via `lgdt`
- segment descriptors for **32‑bit code** and data exist

Only after these conditions are met, the **PE (Protection Enable)** bit in `cr0` is set.

---

## Memory layout excerpt during mode switch

<h6>
    <strong> Note: </strong> <br>
    This is a simplified excerpt of the memory layout of MoleculeOS. <br>
    Only the regions relevant for the mode switch.
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
    +---------------------------+ 0x890
    | ↑↑↑                       |
    | Stack (grows downward)    |
    +---------------------------+ 0x9C00 ← Stackpointer (SP)
    | Free / conventional RAM   |
    | ...                       |
    +---------------------------+ 0xA000
```

The stack and GDT are both available and stable when the mode switch occurs.

---

## Code: Switching to protected mode in MoleculeOS

``` asm
    mov eax, cr0    ; Control => Accumulator (32 Bit)
    or al, 1        ; set Protection Enable
    mov cr0, eax    ; Protection Enable => Control
```

---

Explanation of the control register change
The transition to protected mode is controlled via the CR0 register:

- Bit 0 (PE – Protection Enable)
- 0 → real mode
- 1 → protected mode


Afterwards, the CPU interprets segment selectors using the GDT.

After enabling PE, the CPU is technically in protected mode, but continues to execute instructions as if it were in 16-bit mode. <br>
Once CR0.PE = 1 is set, a far jump to a 32-bit code segment is required to fully switch to 32-bit execution.