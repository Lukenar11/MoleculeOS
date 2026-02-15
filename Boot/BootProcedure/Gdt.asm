align 8

gdt: 
    ; Null descriptor
    gdt_null:
        dq 0x00

    ; Code-Segment descriptor
    gdt_code:
        dw 0xFFFF   ; Limit low
        dw 0x0000   ; Base low
        db 0x00     ; Base middle
        db 0x9A     ; Access
        db 0xCF     ; Flags (4K, 32-bit) + Limit high
        db 0x00     ; Base high

    ; Data-Segment descriptor
    gdt_data:
        dw 0xFFFF   ; Limit low
        dw 0x0000   ; Base low
        db 0x00     ; Base middle
        db 0x92     ; Access
        db 0xCF     ; Flags (4K, 32-bit) + Limit high
        db 0x00     ; Base high

    gdt_end:

GdtDescriptor:
    dw gdt_end - gdt - 1
    dd gdt
