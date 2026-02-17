# Initialization of the Global Descriptor Table (GDT)

This section describes how MoleculeOS sets up the Global Descriptor Table (GDT) during the boot procedure, before switching from 16‑bit real mode into 32‑bit protected mode. <br> 
The GDT is a fundamental structure of the x86 architecture and must be initialized correctly before enabling protected mode.

---

## Purpose of the GDT

The GDT defines memory segments for the CPU. <br>
In real mode, segmentation is fixed by the BIOS, but in protected mode the OS must provide its own segmentation model.

MoleculeOS uses a minimal and clean GDT layout:

- a null descriptor (required by the CPU)
- a code segment (32‑bit, ring 0)
- a data segment (32‑bit, ring 0)

This setup provides a flat memory model where all segments span the full 4 GB address space.

---

## Structure of a Segment Descriptor

Each GDT entry is 8 bytes long and follows this layout:

- Bytes 0–1: Segment Limit (bits 0–15)
- Bytes 2–3: Base Address (bits 0–15)
- Byte 4: Base Address (bits 16–23)
- Byte 5: Access Byte
- Byte 6: Flags + Segment Limit (bits 16–19)
- Byte 7: Base Address (bits 24–31)

The GDT is placed in conventional RAM and loaded using the `lgdt` instruction.

---

## Memory Layout Including the GDT

<h6>
    <strong> Note: </strong> <br>
    This is a simplified excerpt of the memory layout of MoleculeOS. <br>
    Only the areas relevant for initializing the GDT are shown here.
</h6>

``` text
    +---------------------------+ 0x7C00
    | Bootloader (512 Bytes)    |
    | MoleculeOS Boot Sector    |
    +---------------------------+ 0x7E00
    | Global Descriptor Table   |
    | gdt, gdt_end, descriptor  |
    +---------------------------+ 0x7E20
    | Free / conventional RAM   |
    | ...                       |
    +---------------------------+ 0xA000
```

---

## MoleculeOS GDT (Detailed Version)
MoleculeOS uses a readable, non‑compact GDT definition to make each field explicit.

```asm
    align 8

    gdt: 
        ; Null descriptor (GDT index 0: 0x00)
        gdt_null:
            dq 0x00

        ; Code-Segment descriptor (GDT index 1: 0x08)
        gdt_code:
            dw 0xFFFF   ; Limit low
            dw 0x0000   ; Base low
            db 0x00     ; Base middle
            db 0x9A     ; Access
            db 0xCF     ; Flags (4K, 32-bit) + Limit high
            db 0x00     ; Base high

        ; Data-Segment descriptor (GDT index 2: 0x10)
        gdt_data:
            dw 0xFFFF   ; Limit low
            dw 0x0000   ; Base low
            db 0x00     ; Base middle
            db 0x92     ; Access
            db 0xCF     ; Flags (4K, 32-bit) + Limit high
            db 0x00     ; Base high

        gdt_end:

    GdtDescriptor:
        dw gdt_end - gdt - 1
        dd gdt
```

---

## Loading the GDT

Before switching to protected mode, the bootloader loads the GDT using:

``` asm 
    ; _init_ GDT
    lgdt [GdtDescriptor]
```

This loads the GDT into the GDTR register, which the CPU uses to resolve segment selectors.

---

## Summary

- The GDT is required for protected mode.
- MoleculeOS uses a simple, flat memory model.
- Each descriptor is 8 bytes and defines base, limit, access rights, and flags.
- The GDT is loaded with lgdt before enabling protected mode.

This GDT setup forms the foundation for entering 32‑bit mode and starting the MoleculeOS kernel.
