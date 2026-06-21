;
; LICENSE:
;     Copyright (c) 2026 Lukenar11 (Luke Matthes)
;     MIT Licensed
;     https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
; 
; DESCRIPTION:
;     This declares the low-level routines for performing x86 port
;     I/O operations. 
;     
;     These routines provide byte- and word-level access
;     to hardware I/O ports and are used by drivers such as the PS/2
;     controller, PIC, PIT, and VGA hardware.
; 
; NOTES:
;     The C interface is a header file called "halt.asm"
;

global byte_input
global word_input
global byte_output
global word_output

section .text

byte_input:
    mov dx, [esp+4] ; port
    in al, dx       ; uint8_t
    ret

word_input:
    mov dx, [esp+4] ; port
    in ax, dx       ; uint16_t
    ret

byte_output:
    mov dx, [esp+4] ; port
    mov al, [esp+8] ; value
    out dx, al
    ret

word_output:
    mov dx, [esp+4] ; port
    mov ax, [esp+8] ; value
    out dx, ax
    ret
