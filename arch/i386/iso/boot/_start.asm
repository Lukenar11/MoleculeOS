;
; LICENSE:
;     Copyright (c) 2026 Lukenar11 (Luke Matthes)
;     MIT Licensed
;     https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
;
; DESCRIPTION:
;     This file defines the entry point (_start) 
;     that is called by an external boot loader (GRUB).
;
;     This entry point initializes the GDT and the stack, 
;     and starts the kernel by calling 'kernel_main'.
;
; NOTES:
;     The entry point must be named '_start', as GRUB expects it to be.
;

extern kernel_main
extern stack_bottom
extern stack_top
global _start

%define KERNEL_DATA_SEGMENT_SELECTOR 0x10

section .gdt
align 8

gdt_start:
    ; Null Descriptor
    dw 0x0000   ; Limit (bits 0..15)
    dw 0x0000   ; Base (bits 0..15)
    db 0x00     ; Base (bits 16..23)
    db 0x00     ; Access
    db 0x00     ; Flags + Limit (bits 16..19)
    db 0x00     ; Base (bits 24..31)

    ; Kernel Code Segment
    dw 0xFFFF   ; Limit (bits 0..15)
    dw 0x0000   ; Base (bits 0..15)
    db 0x00     ; Base (bits 16..23)
    db 0x9A     ; Access
    db 0xCF     ; Flags + Limit (bits 16..19)
    db 0x00     ; Base (bits 24..31)

    ; Kernel Data Segment
    dw 0xFFFF   ; Limit (bits 0..15)
    dw 0x0000   ; Base (bits 0..15)
    db 0x00     ; Base (bits 16..23)
    db 0x92     ; Access
    db 0xCF     ; Flags + Limit (bits 16..19)
    db 0x00     ; Base (bits 24..31)
gdt_end:

gdt_descriptor:
    dw gdt_end-gdt_start-1
    dd gdt_start

section .text

_start:
    cli

    lgdt [gdt_descriptor]

    mov ax, KERNEL_DATA_SEGMENT_SELECTOR
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    jmp 0x08:.flush

.flush:
    mov esp, stack_top
    jmp kernel_main
    