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

global inb
global outb
global inw
global outw

section .text

; uint8_t inb(uint16_t port)
inb:
    mov dx, [esp+4] ; port
    in al, dx       ; uint8_t
    ret

; void outb(uint16_t port, uint8_t value);
outb:
    mov dx, [esp+4] ; port
    mov al, [esp+8] ; value
    out dx, al
    ret

; uint16_t inw(uint16_t port)
inw:
    mov dx, [esp+4] ; port
    in ax, dx       ; uint16_t
    ret

; void outw(uint16_t port, uint16_t value)
outw:
    mov dx, [esp+4] ; port
    mov ax, [esp+8] ; value
    out dx, ax
    ret
