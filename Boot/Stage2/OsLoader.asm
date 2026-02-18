[bits 32]

global OSLoader 

; kernel::main (C++ Function)
extern main

section .text
    OSLoader:
        ; new Stack-Size (~0.5 MiB)
        mov esp, 0x83F00

        ; _start_ Kernel
        call main

        .hang:
            hlt
            jmp .hang
