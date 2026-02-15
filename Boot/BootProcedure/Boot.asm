[org 0x7C00]
[bits 16]

jmp BootProcedure

%include "Boot/BootProcedure/Gdt.asm"
%include "Boot/BootProcedure/ProtectedModeEntry.asm"

BootProcedure:
    ; Interrupts off
    cli

    ; _init_ Stack
    xor ax, ax      ; clear Accumulator
    mov ss, ax      ; set Stack-Segment
    mov sp, 0x9C00  ; set Stack-Size (8 KB)

    ; _init_ GDT
    lgdt [GdtDescriptor]

    ; _init_ Protected-Mode
    mov eax, cr0    ; Control => Accumulator (32 Bit)
    or al, 1        ; set Protection Enable
    mov cr0, eax    ; Protection Enable => Control

    call 0x08:ProtectedModeEntry

times 510-($-$$) db 0
dw 0xAA55
