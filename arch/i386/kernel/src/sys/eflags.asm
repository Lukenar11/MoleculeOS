;
; LICENSE:
;    Copyright (c) 2026 Lukenar11 (Luke Matthes)
;    MIT Licensed
;    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
;
; DESCRIPTION:
;    This file contains 2 simple assembly routines that 
;    work using stack operations.
;
;    The function 'save_eflags' saves the current CPU flags (flag register).
;    The function 'restore_eflags' restores the CPU flags (flag register).
;
; NOTES:
;    The interface is a header file called 'eflags.hpp'.
;

global save_eflags
global restore_eflags

section .text

save_eflags:
    pushf
    pop eax
    ret

restore_eflags:
    push dword [esp+4]
    popf
    ret
