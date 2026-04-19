global ISRCommonStub
extern isr_common_handler

section .text

ISRCommonStub:
    cli

    ; Save segments
    push ds
    push es
    push fs
    push gs

    ; Load kernel data segment
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Save general purpose registers
    pusha

    ; Pass pointer to TrapFrame
    mov eax, esp
    push eax

    call isr_common_handler
    add esp, 4

    ; restore registers
    popa

    ; restore segments
    pop gs
    pop fs
    pop es
    pop ds

    ; remove error_code + interrupt_number
    add esp, 8

    iretd
    