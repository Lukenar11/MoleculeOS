;
; LICENSE:
;     Copyright (c) 2026 Lukenar11 (Luke Matthes)
;     MIT Licensed
;     https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
;
; DESCRIPTION:
;     This file contains the kernel entry point "_start", which is the first
;     executed instruction after the bootloader transfers control to
;     the kernel.
;
;     The routine loads the Global Descriptor Table (GDT), initializes the
;     kernel stack, aligns it to a 16-byte boundary, and then jumps into
;     the C++ high-level entry point "kernel_main".
;
; NOTES:
;     The internal implementation of "kernel_stack_top" is located in "kernel_stack.asm".
;     The internal implementation of "load_gdt" is located in "gdt.asm".
;

global _start

extern kernel_main
extern kernel_stack_top
extern load_gdt

%define STACK_ALIGN_16_MASK 0xFFFFFFF0

section .text

_start:
    cli

    call load_gdt

    mov esp, kernel_stack_top
    and esp, STACK_ALIGN_16_MASK

    jmp kernel_main
    