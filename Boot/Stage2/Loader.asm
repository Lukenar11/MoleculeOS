global Loader 
extern kernel_main

section .text

    Loader:
        ; Interrupts off
        cli

        mov dword [0xB8000], 0x1F004B41
        
        ; _init_ Kernel-Stack
        mov esp, KernelStackTop ; [StackAddress] => StackPointer
        and esp, 0xFFFFFFF0     ; align to 16 bytes

        ; _start_ Kernel
        jmp kernel_main
    
%include "Boot/Stage2/KernelStack.asm"
