global KernelEntry

extern KernelStackTop
extern kernel_main

section .text

    KernelEntry:
        ; _init_ Kernel-Stack (0.5 MiB)
        mov esp, KernelStackTop ; set Kernel-Stack Size (0.5 MiB)
        mov ebp, esp            ; set Stack-Frame Pointer

        ; _start_ Kernel
        call kernel_main
    