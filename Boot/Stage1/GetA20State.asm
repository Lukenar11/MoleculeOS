GetA20State:
    ; Falgs => Stack
    pushf

    ; Register => Stack
    push ds
    push es
    push di
    push si

    ; Interrupts off
    cli

    ; pointer to [0x00000]
    xor ax, ax  ; Accumulator = 0
    mov es, ax  ; Accumulator => ExtraSegment

    ; Data-Segment = 0xFFFF -> points to [0xFFFF0] (16 bytes below 1 MiB)
    not ax      ; Accumulator = 0xFFFF
    mov ds, ax  ; Data-Segment = 0xFFFF -> if (A20Enabled) Data-Segment = 0xFFFF0

    ; Choose two test addresses
    mov di, 0x0500  ; Address below 1 MiB
    mov si, 0x0510  ; Address above 1 MiB

    ; 4. Save original bytes at both addresses
    mov al, [es:di] ; Read Byte at 0x0500
    push ax         ; Accumulator => Stack

    mov al, [ds:si] ; read Byte <= [0x100000]
    push ax         ; Accumulator => Stack

    ; Write test pattern
    mov byte [es:di], 0x00  ; write 0x00 => [0x0500]
    mov byte [ds:si], 0xFF  ; write 0xFF => [0x100000]

    ; if (!A20Enabled) return (0x100000 == 0x000000)
    cmp byte [es:di], 0xFF

    ; reset Bytes
    pop ax                  ; Stack => Accumulator
    mov byte [ds:si], al    ; write Accumulator => [DS:SI]

    pop ax                  ; Stack => Accumulator
    mov byte [es:di], al    ; write Accumulator => [ES:SI]

    ; return (!A20Enabled) ? 0 : 1
    mov ax, 0
    je done     ; if (!A20Enabled) goto: done
    mov ax, 1   ; else return 1

done:
    ; Stack => Register
    pop si
    pop di
    pop es
    pop ds

    ; Stack => Falgs
    popf

    ret
    