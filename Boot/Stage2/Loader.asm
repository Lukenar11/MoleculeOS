[bits 32]

global Loader 

; Kernel/Boot/KernelEntry.asm
extern KernelEntry

section .text
    Loader:
        cli                 ; Interrupts off
        mov esp, 0x00FF     ; reset Stack (0.25 KiB)
        call KernelEntry    ; _init_ Kernel
