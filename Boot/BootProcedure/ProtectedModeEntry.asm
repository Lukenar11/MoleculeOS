[bits 32]

ProtectedModeEntry:
    mov ax, 0x10    ; Data-Segment selector (GDT index 2)
    mov ds, ax      ; Data-Segment
    mov es, ax      ; Extra-Segment
    mov fs, ax      ; General-Purpose segment
    mov gs, ax      ; General-Purpose segment
    mov ss, ax      ; new Stack-Segment (32-bit)

    ; new Stack-Size (~1 MiB)
    mov esp, 0x100000

    .hang:
        jmp .hang
