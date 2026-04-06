global Loader 
extern kernel_main

section .text

    Loader:
        ; Interrupts off
        cli

        ; _init_ Kernel-Stack
        mov esp, KernelStackTop
        and esp, 0xFFFFFFF0     ; align to 16 bytes

        ; _start_ Kernel
        call kernel_main

        .hang:
            cli
            hlt
            jmp .hang
    
%include "Boot/Stage2/KernelStack.asm"
