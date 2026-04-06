[bits 16]

DiskReadLoader:
    ; save Register
    push ax
    push bx
    push cx
    push dx
    push es

    mov ax, 0x1000  ; Load target segment
    mov es, ax      ; Load sectors from address 0x1000.
    xor bx, bx      ; Offset = 0

    ; Retry counter
    mov si, 3

    .disk_retry:
        mov ah, 0x02    ; BIOS: Read Sectors
        mov al, 0x0F    ; Number of sectors
        mov ch, 0x00    ; Cylinder 0
        mov cl, 0x02    ; Start at sector 2
        mov dh, 0x00    ; Head 0

        int 0x13        ; BIOS: disk read
        jnc .disk_ok    ; if (!CARRY) goto: .disk_ok

        ; Error: Disk reset und Retry
        mov ah, 0x00    ; BIOS: Reset Disk System
        int 0x13        ; BIOS: disk read

        dec si          ; Retry counter - 1
        jnz .disk_retry ; if (!ZERO) goto: .disk_retry

        ; if (fail_count == 3) goto: disk_error
        jmp disk_error

    .disk_ok:
        ; reset Register
        pop es
        pop dx
        pop cx
        pop bx
        pop ax
        ret

disk_error:
    cli
    .hang:
        hlt
        jmp .hang
