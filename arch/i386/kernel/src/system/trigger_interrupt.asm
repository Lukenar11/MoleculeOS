;
; LICENSE:
;     Copyright (c) 2026 Lukenar11 (Luke Matthes)
;     MIT Licensed
;     https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
;
; DESCRIPTION:
;     This file creates a table with 256 entries at assembly time, 
;     where each entry is a function that triggers an interrupt.
;     
;     The function 'trigger_interrupt' takes an integer, 
;     uses it to determine which interrupt table entry it needs to call, 
;     and jumps to that entry. 
;
; NOTES:
;

global trigger_interrupt

%define INTERRUPT_TABLE_SIZE 256
%define FIRST_INTERRUPT_TABLE_ENTRY 0
%define LAST_INTERRUPT_TABLE_ENTRY 255

section .text

interrupt_table:
%assign i FIRST_INTERRUPT_TABLE_ENTRY
%rep INTERRUPT_TABLE_SIZE
dd int_%+i
%assign i i+1
%endrep

%assign i FIRST_INTERRUPT_TABLE_ENTRY
%rep INTERRUPT_TABLE_SIZE
int_%+i:
    int i
    ret
%assign i i+1
%endrep

trigger_interrupt:
    mov eax, [esp+4]
    cmp eax, LAST_INTERRUPT_TABLE_ENTRY
    ja  .done
    jmp [interrupt_table+eax*4]

.done:
    ret
