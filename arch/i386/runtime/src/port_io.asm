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
;     The interface is a header file called 'port_io.hpp'
;

global byte_input
global word_input
global word_input_stream
global byte_output
global word_output
global word_output_stream

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
    push edi

    mov  dx, [esp+8]
    mov ecx, [esp+12]
    mov edi, [esp+16]
    cld
    rep insw

    pop edi
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

word_output_stream:
    push esi

    mov  dx, [esp+8]
    mov ecx, [esp+12]
    mov esi, [esp+16]
    cld
    rep outsw

    pop esi
    ret
