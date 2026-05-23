;
; LICENSE:
;     Copyright (c) 2026 Lukenar11 (Luke Matthes)
;     MIT Licensed
;     https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
;
; DESCRIPTION:
;     This file exposes the low-level CPU interrupt enebling routine.
;     The routine uses the i386 instruction "sti" to disable interrupts.
;
; NOTES:
;     The C interface is a header file called "kernel_system_enable_interrupts.h"
;

global kernel_system_enable_interrupts

section .text

; void kernel_system_enable_interrupts(void)
kernel_system_enable_interrupts:
    sti
    ret
