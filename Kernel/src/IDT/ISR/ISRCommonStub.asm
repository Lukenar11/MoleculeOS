global ISRCommonStub
extern isr_common_handler

section .text
    
ISRCommonStub:
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
    call isr_common_handler
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
    