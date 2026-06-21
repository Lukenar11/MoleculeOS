;
; LICENSE:
;     Copyright (c) 2026 Lukenar11 (Luke Matthes)
;     MIT Licensed
;     https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
;
; DESCRIPTION:
;     This file exposes the low-level CPU sleep routine.
;
;     This routine uses the i386 instruction "halt"
;     to stop the CPU until an interrupt occurs.
;
; NOTES:
;     The interface is a header file called "sleep.hpp"
;

global sleep

section .text
sleep:
    hlt
    ret
