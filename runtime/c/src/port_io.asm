;
;    LICENSE:
;        Copyright (c) 2026 Lukenar11 (Luke Matthes)
;        MIT Licensed
;        https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
;    
;    DESCRIPTION:
;        This declares the low-level routines for performing x86 port
;        I/O operations. 
;        
;        These routines provide byte- and word-level access
;        to hardware I/O ports and are used by drivers such as the PS/2
;        controller, PIC, PIT, and VGA hardware.
;    
;    NOTES:
;        The C interface is a header file called "halt.asm"
;

global inb
global outb
global inw
global outw

section .text

inb:
    mov dx, [esp+4] ; Parameter 1: port
    in al, dx
    ret

outb:
    mov dx, [esp+4] ; Parameter 1: port
    mov al, [esp+8] ; Parameter 2: value
    out dx, al
    ret

inw:
    mov dx, [esp+4] ; Parameter 1: port
    in ax, dx
    ret

outw:
    mov dx, [esp+4] ; Parameter 1: port
    mov ax, [esp+8] ; Parameter 2: value
    out dx, ax
    ret
