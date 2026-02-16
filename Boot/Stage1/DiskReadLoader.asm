[bits 16]

DiskReadLoader:
    mov ax, 0x1000  ; Load target segment
    mov es, ax
    xor bx, bx      ; Offset = 0

    mov ah, 0x02    ; BIOS: Read-Sectors
    mov al, 16      ; Number of sectors
    mov ch, 0       ; Cylinder 0
    mov cl, 2       ; Start at sector 2 (sector 1 = Boot-Sector)
    mov dh, 0       ; Head 0
    mov dl, 0       ; Drive 0 (Floppy A)

    int 0x13        ; Perform disk read
    jc disk_error   ; if (CARRY == true) goto: disk_error

    ret

disk_error:
    halt
    jmp disk_error
