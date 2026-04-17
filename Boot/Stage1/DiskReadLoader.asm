DiskReadLoader:
    push ax
    push bx
    push cx
    push dx
    push es

    mov ax, 0x1000
    mov es, ax
    xor bx, bx

    mov ah, 0x02
    mov al, 0x10
    mov ch, 0x00
    mov cl, 0x01
    mov dh, 0x00
    mov dl, [BOOT_DRIVE]
    int 0x13
    jc disk_error

disk_ok:
    pop es
    pop dx
    pop cx
    pop bx
    pop ax
    ret

disk_error:
    mov ah, 0x0E
    mov al, 'E'
    int 0x10
.hang:
    cli
    hlt
    jmp .hang
