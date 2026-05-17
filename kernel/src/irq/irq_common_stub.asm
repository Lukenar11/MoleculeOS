;
;    LICENSE:
;        Copyright (c) 2026 Lukenar11 (Luke Matthes)
;        MIT License
;        https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
;
;    DESCRIPTION:
;        This routine saves the full CPU register state, switches to the kernel
;        data segment, and forwards control to the "irq_common_handler".
;
;    NOTES:
;        The stack layout created here must match the "RegisterDump" structure
;        exactly. 
;
;        Any deviation will corrupt the register dump and may cause
;        undefined behavior.
;
;        After the handler returns, this stub restores all registers, removes
;        the pushed interrupt number and error code, and returns using "iretd".
;

global irq_common_stub

extern irq_common_handler

section .text
    
irq_common_stub:
    ; Save: general purpose register
    push eax
    push ecx
    push edx
    push ebx
    push ebp
    push esi
    push edi

    ; Save: segmentregister
    push gs
    push fs
    push es
    push ds

    ; set: kernel data segment
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; pointer to RegisterDump
    push esp
    call irq_common_handler
    add esp, 4

    ; reset: segmentregister
    pop ds
    pop es
    pop fs
    pop gs

    ; reset: general purpose register
    pop edi
    pop esi
    pop ebp
    pop ebx
    pop edx
    pop ecx
    pop eax

    ; remove: interrupt_number + error_code
    add esp, 8

    iretd
    