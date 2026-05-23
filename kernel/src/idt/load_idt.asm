;
; LICENSE:
;     Copyright (c) 2026 Lukenar11 (Luke Matthes)
;     MIT licensed
;     https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
;
; DESCRIPTION:
;     This is a simple assembly routine for loading the IDT.
;
;     This routine uses the i386 and x86/64 instruction "lidt",
;     which tells the CPU where the IDT is located and that it should be loaded.
;
; NOTES:
;     The C interface is a header file called "load_idt.h".
;

global load_idt

section .text

; void load_idt(uintptr_t)
load_idt:
    mov eax, [esp+4]    ; uintptr_t
    lidt [eax]
    ret 
