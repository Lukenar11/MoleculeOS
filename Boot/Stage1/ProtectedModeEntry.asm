[bits 32]

ProtectedModeEntry:
    mov ax, 0x10    ; Data-Segment selector (GDT index 2)
    mov ds, ax      ; Data-Segment
    mov es, ax      ; Extra-Segment
    mov fs, ax      ; General-Purpose segment
    mov gs, ax      ; General-Purpose segment
    mov ss, ax      ; new Stack-Segment (32-bit)

    ; _start_ OSLoader (Stage 2) (fully 32-Bit Mode)
    jmp 0x08:0x7E00
