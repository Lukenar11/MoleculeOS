global HaltSystem

section .text

HaltSystem:
    .hang:
        cli
        hlt
        jmp .hang
