global Loader 

extern KernelStackTop
extern kernel_main

section .text

    Loader:
        ; Interrupts off
        cli

        ; _init_ Kernel-Stack
        mov esp, KernelStackTop ; set Kernel-Stack Size (4 KiB)
        mov ebp, esp            ; set Stack-Frame Pointer

        ; _start_ Kernel
        call kernel_main
    