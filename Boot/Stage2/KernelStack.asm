section .bss

    ; Kernel-Stack (4 KiB)
    kernel_stack_bottom: 
        resb 4 * 1024

    KernelStackTop:
    