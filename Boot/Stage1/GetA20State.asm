GetA20State:
    ; save Flags
    pushf

    ; save Register
    push ds
    push es
    push di
    push si

    ; Interrupts off
    cli

    ; pointer to [0x00000]
    xor ax, ax  ; AX = 0
    mov es, ax  ; AX => ES

    ; DS = 0xFFFF -> points to [0xFFFF0] (16 bytes below 1 MiB)
    not ax      ; AX = 0xFFFF
    mov ds, ax  ; DS = 0xFFFF -> if (A20Enabled) DS = 0xFFFF0

    ; Choose two test addresses
    mov di, 0x0500  ; Address below 1 MiB
    mov si, 0x0510  ; Address above 1 MiB

    ; 4. Save original bytes at both addresses
    mov al, [es:di] ; Read Byte at 0x0500
    push ax         ; save Byte

    mov al, [ds:si] ; Read Byte at [0x100000]
    push ax         ; save Byte

    ; Write test pattern
    mov byte [es:di], 0x00  ; write 0x00 => [0x0500]
    mov byte [ds:si], 0xFF  ; write 0xFF => [0x100000]

    ; if (!A20Enabled) return (0x100000 == 0x000000)
    cmp byte [es:di], 0xFF

    ; reset Bytes
    pop ax                  ; Stack => AX
    mov byte [ds:si], al    ; AL => [DS:SI]

    pop ax                  ; Stack => AX
    mov byte [es:di], al    ; AL => [ES:SI]


    ; return (!A20Enabled) ? 0 : 1
    mov ax, 0
    je done     ; if (!A20Enabled) goto: done
    mov ax, 1   ; else return 1

done:
    ; reset Register
    pop si
    pop di
    pop es
    pop ds

    ; reset Falgs
    popf

    ret
    