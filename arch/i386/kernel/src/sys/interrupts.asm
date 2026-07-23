;
; LICENSE:
;    Copyright (c) 2026 Lukenar11 (Luke Matthes)
;    MIT Licensed
;    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
;
; DESCRIPTION:
;    This file exposes the low-level CPU interrupt enabling routine.
;    The routine uses the i386 instruction "sti" to disable interrupts.
;
; NOTES:
;    The interface is a header file called 'interrupts.hpp'.
;

global enable_interrupts
global trigger_interrupt

%define INTERRUPT_TABLE_SIZE 256
%define FIRST_INTERRUPT_TABLE_ENTRY 0
%define LAST_INTERRUPT_TABLE_ENTRY 255

section .text

enable_interrupts:
    sti
    ret

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
