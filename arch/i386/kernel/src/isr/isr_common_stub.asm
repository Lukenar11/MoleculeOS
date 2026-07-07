;
; LICENSE:
;     Copyright (c) 2026 Lukenar11 (Luke Matthes)
;     MIT License
;     https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
;
; DESCRIPTION:
;      This routine saves the full CPU register state, switches to the kernel
;      data segment, and forwards control to the "isr_common_handler".
;
; NOTES:
;     The stack layout created here must match the "RegisterDump" structure
;     exactly. 
;
;     Any deviation will corrupt the register dump and may cause
;     undefined behavior or a triple fault.
;
;     After the handler returns, this stub restores all registers, removes
;     the pushed interrupt number and error code, and returns using "iretd".
;

extern isr_common_handler
global isr_common_stub

%define KERNEL_DATA_SEGMENT_SELECTOR 0x10

section .text
isr_common_stub:
    push eax
    push ecx
    push edx
    push ebx
    push ebp
    push esi
    push edi

    push gs
    push fs
    push es
    push ds

    ; set kernel data segment
    mov ax, KERNEL_DATA_SEGMENT_SELECTOR
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; pointer to "Register_Dump"
    push esp
    call isr_common_handler
    add esp, 4

    pop ds
    pop es
    pop fs
    pop gs

    pop edi
    pop esi
    pop ebp
    pop ebx
    pop edx
    pop ecx
    pop eax

    ; remove "interrupt_number" + "error_code"
    add esp, 8

    iretd
