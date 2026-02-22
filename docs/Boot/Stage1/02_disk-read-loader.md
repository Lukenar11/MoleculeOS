# Disk Read Loader (Real Mode)

This section describes how MoleculeOS loads the second-stage loader from disk during the early boot process. <br>
The BIOS provides low-level disk access through interrupt `int 0x13`, which is only available in 16‑bit real mode. <br>
Therefore, the disk read operation must be completed before switching to protected mode. <br>

---

## Goal of the Disk Read Loader

The boot sector (Stage 1) is limited to 512 bytes and cannot contain the entire operating system loader. <br>
Its primary responsibilities are:

- initialize the CPU environment (stack, segments)
- load the next part of the OS (Stage 2) from disk
- prepare the transition into protected mode

To achieve this, MoleculeOS uses the BIOS disk service to load a predefined number of sectors from the boot device into memory.

---

## Memory Layout During Disk Loading

<h6>
    <strong>Note:</strong> <br>
    This is a simplified excerpt of the MoleculeOS memory layout. <br>
    Only the regions relevant for disk loading are shown here.
</h6>

``` text
    +---------------------------+ 0x0000
    | Interrupt Vector Table    |
    | BIOS Data Area            |
    +---------------------------+ 0x0500
    | Free / BIOS workspace     |
    +---------------------------+ 0x7C00
    | Bootloader (Stage 1)      |
    | MoleculeOS Boot Sector    |
    +---------------------------+ 0x7E00
    | Stage 2 Load Target       |
    | (loaded by int 0x13)      |
    +---------------------------+
    | Free / conventional RAM   |
    | ...                       |
    +---------------------------+ 0x10000  ← ES:BX = 0x1000:0000
    | tage 2 loaded here        |
    | (16 Sektoren = 8 KiB)     |
    +---------------------------+ 0x12000
```
---

Stage 2 is loaded into a safe memory region that does not overlap with:

- the boot sector
- BIOS memory
- the real‑mode stack
- VGA memory

This ensures that Stage 2 can execute reliably once protected mode is enabled.

---

# Why BIOS Disk Access Must Happen in Real Mode

The BIOS provides disk access through:


`int 0x13` - Disk Services
These services are only available in:

- 16‑bit real mode
- virtual 8086 mode (not used here)
- Once the CPU enters protected mode:
- BIOS interrupts are no longer available
- `int 0x13` cannot be used

disk access must be implemented manually **(ATA, AHCI, NVMe drivers)**

---

Therefore:

All disk loading must be completed before switching to protected mode.
CHS Addressing **(Cylinder / Head / Sector)** <br>
The BIOS uses the CHS model to locate sectors on a disk.

---

## Floppy Disk (CHS-Modell)
``` text
   +--------------------------+
   | Cylinder 0               |
   | +----------------------+ |
   | | Head 0 (Seite 1)     | |
   | |   Sektor 1 (Boot)    | |
   | |   Sektor 2 (Stage 2) | |
   | |   Sektor 3           | |
   | |   ...                | |
   | +----------------------+ |
   | +----------------------+ |
   | | Head 1 (Seite 2)     | |
   | |   Sektor 1           | |
   | |   Sektor 2           | |
   | |   ...                | |
   | +----------------------+ |
   +--------------------------+
```

---

## CHS Parameters Used by MoleculeOS

Register Meaning Value:
- `ch` Cylinder: **0**
- `dh` Head: **0**
- `cl` Sector: **2**
- `al` Number of sectors: **16**

---

This loads:

- **16** sectors
- starting at Cylinder **0**, Head **0**, Sector **2**
- directly after the boot sector
- Where Stage 2 Is Loaded **(es:bx → Physical Address)**

---

## The BIOS writes the loaded sectors to the memory address specified by:

**es:bx** MoleculeOS sets:
``` asm
    mov ax, 0x1000   ; Segment
    mov es, ax
    xor bx, bx       ; Offset = 0
```
---

Physical address calculation:
``` code
    phys = (ES * 16) + BX
    phys = (0x1000 * 16) + 0
    phys = 0x10000
```

---

## Full Disk Read Flow (Visual Overview)

``` text
    +----------------------------+
    | 1. BIOS loadts boot sector |
    |    to 0x7C00               |
    +-------------+--------------+
                  |
                  v
    +----------------------------+
    | 2. Bootloader sets ES:BX   |
    |    ES = 0x1000             |
    |    BX = 0x0000             |
    +-------------+--------------+
                  |
                  v
    +----------------------------+
    | 3. Bootloader sets CHS     |
    |    CH = 0, CL = 2, DH = 0  |
    +-------------+--------------+
                  |
                  v
    +----------------------------+
    | 4. int 0x13                |
    |    BIOS reads 16 sectors   |
    |    to ES:BX (0x10000)      |
    +-------------+--------------+
                  |
                  v
    +----------------------------+
    | 5. Stage 2 is in RAM       |
    |    and can be executed     |
    +----------------------------+
```

---

## Disk Read Loader Code (MoleculeOS)

``` asm
    [bits 16]

    DiskReadLoader:
        mov ax, 0x1000  ; Load target segment
        mov es, ax
        xor bx, bx      ; Offset = 0
    
        mov ah, 2       ; BIOS: Read-Sectors
        mov al, 16      ; Number of sectors
        mov ch, 0       ; Cylinder 0
        mov cl, 2       ; Start at sector 2 (sector 1 = Boot-Sector)
        mov dh, 0       ; Head 0
        mov dl, 0       ; Drive 0 (Floppy A)
    
        int 0x13        ; Perform disk read
        jc disk_error   ; if (CARRY == true) goto: disk_error

        ret

    disk_error:
        hlt
        jmp disk_error
```
---

## Why Stage 2 Starts at Sector 2

The BIOS loads the boot sector (Stage 1) from:

- Code
- Cylinder **0**, Head **0**, Sector **1**
- Everything after that is free for the OS to use.

---

MoleculeOS places its Stage 2 loader directly after the boot sector:

- Code
- Sector 2 → Stage 2 (OsLoader)
- Sector 3 → continuation

This makes the loading process simple and deterministic.

---

## Summary

The Disk Read Loader is a critical part of Stage 1.
It ensures that MoleculeOS can:

- load its second‑stage loader from disk
- prepare for protected mode
- transition into 32‑bit execution
- Without this step, the OS would have no way to load additional code beyond the 512‑byte boot sector.
