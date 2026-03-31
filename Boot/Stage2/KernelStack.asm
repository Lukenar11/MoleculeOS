align 16

global KernelStackTop

section .bss

    ; Kernel-Stack (0.5 MiB)
    KernelStackBottom: 
        resb 512 * 1024

    KernelStackTop:
