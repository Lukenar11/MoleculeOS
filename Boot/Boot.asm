[bits 32]

global Boot
extern kernel_main
extern StackTop
extern kernel_panic

section .rodata

panic_msessage: db "kernel_main returned unexpectedly!", 10, \
                   "Please restart your computer!", 0

section .text

Boot:
    cli

    mov esp, StackTop
    and esp, 0xFFFFFFF0

    sti
    call kernel_main

    .hang:
        push panic_msessage
        call kernel_panic
