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
;     The interface is a header file called "enable_interrupts.hpp"
;

global enable_interrupts

section .text

enable_interrupts:
    sti
    ret
