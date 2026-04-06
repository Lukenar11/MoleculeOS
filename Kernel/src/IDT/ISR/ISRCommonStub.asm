global ISRCommonStub
extern isr_common_handler

section .text
    
    ISRCommonStub:
    ; General Purpose Register => Stack
    push eax
    push ecx
    push edx
    push ebx
    push ebp
    push esi
    push edi

    ; Segmentregister => Stack
    push gs
    push fs
    push es
    push ds

    ; set Kernel Data Segment
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Pointer to RegisterDump
    push esp
    call isr_common_handler
    add esp, 4

    ; Stack => Segmentregister
    pop ds
    pop es
    pop fs
    pop gs

    ; Stack => General Purpose Register
    pop edi
    pop esi
    pop ebp
    pop ebx
    pop edx
    pop ecx
    pop eax

    ; remove interrupt_number + error_code
    add esp, 8

    iretd
    