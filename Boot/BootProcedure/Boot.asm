[org 0x7C00]
[bits 16]

jmp BootProcedure

%include "Boot/BootProcedure/Gdt.asm"

BootProcedure:
    ; Interrupts off
    cli

    ; _init_ Stack
    xor ax, ax      ; clear Accumulator
    mov ss, ax      ; set Stack-Segment
    mov sp, 0x9C00  ; set Stack-Size (8 KB)

    ; _init_ GDT
    lgdt [GdtDescriptor]

    ; Interrupts on
    sti
    halt

times 510-($-$$) db 0
dw 0xAA55
