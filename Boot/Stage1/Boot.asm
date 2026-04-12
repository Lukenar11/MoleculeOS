[org 0x7C00]
[bits 16]

jmp BootProcedure

BootProcedure:
    ; Interrupts off
    cli

    ; _init_ Stack
    xor ax, ax      ; clear Accumulator
    mov ss, ax      ; set Stack-Segment
    mov sp, 0x7BFE  ; set Stack-Size (~7 KiB)
    mov ax, ax      ; nop

    ; _init_ Disk-Loader
    call DiskReadLoader

    ; _init_ GDT
    lgdt [GDTDescriptor]

    ; _start_ A20-Line
    call GetA20State
    test ax, ax

    jnz .a20_done   ; if (Accumulator != 0) goto: .a20_done

    in al, 0x92     ; Control-Port A => Accumulator
    or al, 0x02     ; set A20 Enable
    out 0x92, al    ; Accumulator => Control-Port A

    .a20_done:

    ; _init_ Protected-Mode
    mov eax, cr0    ; Control => Accumulator (32-bit)
    or eax, 0x01    ; set Protection Enable
    mov cr0, eax    ; Protection Enable => Control

    ; _start_ Protected-Mode
    jmp 0x08:ProtectedModeEntry

%include "Boot/Stage1/GDT.asm"
%include "Boot/Stage1/GetA20State.asm"
%include "Boot/Stage1/ProtectedModeEntry.asm"
%include "Boot/Stage1/DiskReadLoader.asm"

times 510 - ($ - $$) db 0x00
dw 0xAA55
