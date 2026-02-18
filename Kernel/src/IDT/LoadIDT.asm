global LoadIDT

section .text
    LoadIDT:
        mov eax, [esp+4]    ; Stack-Address + 4 Bytes => Accumulator
        lidt [eax]          ; _init_ IDT
        ret
