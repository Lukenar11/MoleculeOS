global LoadIDT

section .text

LoadIDT:
    mov eax, [esp + 4]
    lidt [eax]
    ret 
