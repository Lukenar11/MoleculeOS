global KernelEntry

extern kernel_stack_top
extern main

section .text
    KernelEntry:
        ; _init_ Kernel-Stack (0.5 MiB)
        mov esp, kernel_stack_top   ; set Kernel-Stack Size (0.5 MiB)
        mov ebp, esp                ; set Stack-Frame Pointer

        ; _start_ C++-Kernel (kernel::main())
        call main
    