;
; LICENSE:
;     Copyright (c) 2026 Lukenar11 (Luke Matthes)
;     MIT License
;     https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
;
; DESCRIPTION:
;     Each function corresponds to a specific hardware interrupt line 
;     of the Programmable Interrupt Controller (PIC).
;
; NOTES:
;     The C interface is a header file called "irq.h".
;

extern irq_common_handler
extern irq_common_stub
global irq_0, irq_1, irq_2, irq_3, irq_4, irq_5, irq_6, irq_7, \
       irq_8, irq_9, irq_10, irq_11, irq_12, irq_13, irq_14, irq_15

section .text

%macro IRQ 2
irq_%1:
    cli
    push dword 0    ; error code
    push dword %2   ; interrupt number
    jmp irq_common_stub
%endmacro

IRQ 0, 32   ; (irq_0) Timer Interrupt
IRQ 1, 33   ; (irq_1) Keyboard Interrupt
IRQ 2, 34   ; (irq_2) Cascade Interrupt (used internally by the PIC)
IRQ 3, 35   ; (irq_3) COM2 Interrupt
IRQ 4, 36   ; (irq_4) COM1 Interrupt
IRQ 5, 37   ; (irq_5) LPT2 Interrupt
IRQ 6, 38   ; (irq_6) Floppy Disk Interrupt
IRQ 7, 39   ; (irq_7) LPT1 Interrupt
IRQ 8, 40   ; (irq_8) Real-Time Clock Interrupt
IRQ 9, 41   ; (irq_9) Free for peripherals (usually used for ACPI)
IRQ 10, 42  ; (irq_10) Free for peripherals (usually used for ACPI)
IRQ 11, 43  ; (irq_11) Free for peripherals (usually used for ACPI)
IRQ 12, 44  ; (irq_12) PS/2 Mouse Interrupt
IRQ 13, 45  ; (irq_13) FPU Interrupt (Math Coprocessor)
IRQ 14, 46  ; (irq_14) Primary ATA Hard Disk Interrupt
IRQ 15, 47  ; (irq_15) Secondary ATA Hard Disk Interrupt
