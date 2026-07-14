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
;     The interface is a header file called "halt.asm"
;

global byte_input
global word_input
global word_input_stream
global byte_output
global word_output

section .text

byte_input:
    xor eax, eax
    mov  dx, [esp+4]
    in   al, dx
    ret

word_input:
    xor eax, eax
    mov  dx, [esp+4] 
    in   ax, dx
    ret

word_input_stream:
    mov  dx, [esp+4] 
    mov ecx, [esp+8] 
    mov edi, [esp+12]
    rep insw
    ret

byte_output:
    mov dx, [esp+4] 
    mov al, [esp+8]
    out dx, al
    ret

word_output:
    mov dx, [esp+4] 
    mov ax, [esp+8] 
    out dx, ax
    ret
