global kernel_stack_top
align 16

section .bss
    kernel_stack_bottom:
        ; Kernel-Stack (0.5 MiB)
        resb 512 * 1024

    kernel_stack_top:
