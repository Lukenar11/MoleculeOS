[bits 32]
align 16

global StackTop

section .bss

stack_bottom:
    resb 16384
StackTop:
