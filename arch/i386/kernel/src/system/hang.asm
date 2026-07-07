;
; LICENSE:
;     Copyright (c) 2026 Lukenar11 (Luke Matthes)
;     MIT Licensed
;     https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
;
; DESCRIPTION:
;     This file exposes the low-level CPU halt routine.
;
;     The routine disables interrupts and enters an infinite halt loop,
;     ensuring that the CPU remains in a safe, non-executing state.
;
; NOTES:
;     The interface is a header file called "hang.hpp"
;

global hang

section .text

hang:
.halt_loop:
    cli
    hlt
    jmp .halt_loop
