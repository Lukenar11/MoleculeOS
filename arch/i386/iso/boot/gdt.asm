;
; LICENSE:
;     Copyright (c) 2026 Lukenar11 (Luke Matthes)
;     MIT Licensed
;     https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
; 
; DESCRIPTION:
;     This file implements the "Global Descriptor Table" (GDT) 
;     used by the kernel
;     and provides the "load_gdt" routine for loading it into the CPU.
;    
;     The routine installs the GDT via the "lgdt" instruction, reloads all
;     segment registers, and performs a far jump to ensure the new code
;     segment descriptor becomes active.
;     
; NOTES:
;

global load_gdt

%define GDT_NULL_DESCRIPTOR 0x0000000000000000
%define GDT_KERNEL_CODE_SEGMENT 0x00CF9A000000FFFF
%define GDT_KERNEL_DATA_SEGMENT 0x00CF92000000FFFF
%define KERNEL_DATA_SEGMENT_SELECTOR 0x10

section .gdt

gdt_start:
    dq GDT_NULL_DESCRIPTOR
    dq GDT_KERNEL_CODE_SEGMENT
    dq GDT_KERNEL_DATA_SEGMENT
gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

section .text

load_gdt:
    lgdt [gdt_descriptor]

    mov   ax, KERNEL_DATA_SEGMENT_SELECTOR
    mov   ds, ax
    mov   es, ax
    mov   fs, ax
    mov   gs, ax
    mov   ss, ax

    jmp   0x08:.flush

.flush:
    ret
