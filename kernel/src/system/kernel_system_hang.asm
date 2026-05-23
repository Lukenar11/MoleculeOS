;
;    LICENSE:
;        Copyright (c) 2026 Lukenar11 (Luke Matthes)
;        MIT Licensed
;        https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
;
;    DESCRIPTION:
;        This file exposes the low-level CPU halt routine.
;
;        The routine disables interrupts and enters an infinite halt loop,
;        ensuring that the CPU remains in a safe, non-executing state.
;
;    NOTES:
;        The C interface is a header file called "kernel_system_hang.h"
;

global kernel_system_hang

section .text

kernel_system_hang:
.hang:
    cli
    hlt
    jmp .hang
