[org 0x7C00]
[bits 16]

jmp BootProcedure

BootProcedure:
    ; Interrupts off
    cli

    ; _init_ Stack
    xor ax, ax      ; clear Accumulator
    mov ss, ax      ; set Stack-Segment
    mov sp, 0x9C00  ; set Stack-Size (8 KB)

    ; _init_ Disk-Loader
    call DiskReadLoader

    ; _init_ GDT
    lgdt [GdtDescriptor]

    ; _init_ Protected-Mode
    mov eax, cr0    ; Control => Accumulator (32 Bit)
    or eax, 1       ; set Protection Enable
    mov cr0, eax    ; Protection Enable => Control

    ; _start_ Protected-Mode
    call 0x08:ProtectedModeEntry

%include "Boot/Stage1/Gdt.asm"
%include "Boot/Stage1/ProtectedModeEntry.asm"
%include "Boot/Stage1/DiskReadLoader.asm"

times 510-($-$$) db 0
dw 0xAA55
