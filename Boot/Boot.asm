[bits 32]

global Boot
extern kernel_main

section .text
Boot:
    cli

    mov esp, 0x90000
    and esp, 0xFFFFFFF0

    call kernel_main

    .hang:
        cli
        hlt
        jmp .hang
