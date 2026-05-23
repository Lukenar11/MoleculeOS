;
;    LICENSE:
;        Copyright (c) 2026 Lukenar11 (Luke Matthes)
;        MIT Licensed
;        https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
;    
;    DESCRIPTION:
;    	This file contains 2 simple assembly routines that work using stack operations.
;
;        The function "kernel_system_save_eflags" saves the current CPU flags (flag register).
;        The function "kernel_system_restore_eflags" restores the CPU flags (flag register).
;    
;    NOTES:
;    	The C interface is a header file called "cpu_flags.h".
;

global kernel_system_save_eflags
global kernel_system_restore_eflags

section .text

kernel_system_save_eflags:
    pushf
    pop eax
    ret

kernel_system_restore_eflags:
    push dword [esp+4]  ; Parameter: 1
    popf
    ret
