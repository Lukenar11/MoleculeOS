[bits 32]

global Boot
extern kernel_main
extern StackTop

section .text

Boot:
    ; Interrupts off
    cli

    ; _init_ Kernel-Stack
    mov esp, StackTop
    and esp, 0xFFFFFFF0

    call kernel_main
    
    .hang:
        cli
        hlt
        jmp .hang
