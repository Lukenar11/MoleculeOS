;
;    LICENSE:
;        Copyright (c) 2026 Lukenar11 (Luke Matthes)
;        MIT Licensed
;        https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
;
;    DESCRIPTION:
;        This file exposes the low-level CPU sleep routine.
;
;        This routine uses the i386 instruction "halt"
;        to stop the CPU until an interrupt occurs.
;
;    NOTES:
;        The C interface is a header file called "kernel_system_sleep.h"
;

global kernel_system_sleep

section .text

kernel_system_sleep:
    hlt
    ret
