[bits 32]

global OsLoader
jmp OsLoader

section .text
    OsLoader:
        mov edi, 0xB8000

        mov byte [edi], 'H'
        mov byte [edi+1], 0x0F

        mov byte [edi+2], 'e'
        mov byte [edi+3], 0x0F

        mov byte [edi+4], 'l'
        mov byte [edi+5], 0x0F

        mov byte [edi+6], 'l'
        mov byte [edi+7], 0x0F

        mov byte [edi+8], 'o'
        mov byte [edi+9], 0x0F

        .hang:
            jmp .hang
