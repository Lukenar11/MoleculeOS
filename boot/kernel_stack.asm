;    
; LICENSE:
;     Copyright (c) 2026 Lukenar11 (Luke Matthes)
;     MIT Licensed
;     https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
; 
; DESCRIPTION:
;     This file defines the kernel stack used during early system startup
;     and throughout kernel execution.
; 
;     The symbol "kernel_stack_top" marks the initial stack pointer used
;     by the boot entry code before transferring control to "kernel_main".
; 
; NOTES:
;     The stack grows downward, starting at "kernel_stack_top" and moving
;     toward "stack_bottom". 
; 
;     The stack is allocated in the BSS
;     section and aligned to a 16‑byte boundary to satisfy the System V
;     ABI requirements for C and C++ code.
;    

global kernel_stack_top

%define KERNEL_STACK_SIZE (16 * 1024)

section .bss
align 16

stack_bottom:

    resb KERNEL_STACK_SIZE

kernel_stack_top:
