global isr_0, isr_1, isr_2, isr_3, isr_4, isr_5, isr_6, isr_7, \
       isr_8, isr_9, isr_10, isr_11, isr_12, isr_13, isr_14, isr_15, \
       isr_16, isr_17, isr_18, isr_19, isr_20, isr_21, isr_22, isr_23, \
       isr_24, isr_25, isr_26, isr_27, isr_28, isr_29, isr_30, isr_31

extern isr_common_handler

section .text
    %macro NO_ISR_ERROR 1
        isr_%1:
            ; Interrupts off
            cli
    
            push dword 0    ; Error-Code => Stack (32-Bit)
            push dword %1   ; Interrupt-Number => Stack (32-Bit)

            ; kernel::idt::isr_common_handler (C++ Function)
            call isr_common_handler
    
            ; Stack (32-Bit) => 2 Argumentes
            add esp, 8
            iretd
    %endmacro
    
    %macro ISR_ERROR 1
        isr_%1:
            ; Interrupts off
            cli
            push dword 0    ; Error-Code => Stack (32-Bit)
            push dword %1   ; Interrupt-Number => Stack (32-Bit)

            ; kernel::idt::isr_common_handler (C++ Function)
            call isr_common_handler

            ; Stack (32-Bit) => 2 Argumentes
            add esp, 8
            iretd
    %endmacro

    ; _init_ ISR
    NO_ISR_ERROR 0  ; (isr_0 ) Divide Error
    NO_ISR_ERROR 1  ; (isr_1 ) Debug
    NO_ISR_ERROR 2  ; (isr_2 ) NMI
    NO_ISR_ERROR 3  ; (isr_3 ) Breakpoint
    NO_ISR_ERROR 4  ; (isr_4 ) Overflow
    NO_ISR_ERROR 5  ; (isr_5 ) Bound Range Exceeded
    NO_ISR_ERROR 6  ; (isr_6 ) Invalid Opcode
    NO_ISR_ERROR 7  ; (isr_7 ) Device Not Available
    ISR_ERROR 8     ; (isr_8 ) Double Fault (dangerous)
    NO_ISR_ERROR 9  ; (isr_9 ) Coprocessor Segment Overrun
    ISR_ERROR 10    ; (isr_10) Invalid TSS (error code)
    ISR_ERROR 11    ; (isr_11) Segment Not Present (error code)
    ISR_ERROR 12    ; (isr_12) Stack Fault (dangerous)
    ISR_ERROR 13    ; (isr_13) General Protection Fault (error code)
    ISR_ERROR 14    ; (isr_14) Page Fault (dangerous without paging)
    NO_ISR_ERROR 15 ; (isr_15) Reserved (unused)
    NO_ISR_ERROR 16 ; (isr_16) x87 FP Exception
    ISR_ERROR 17    ; (isr_17) Alignment Check (error code, often unsupported)
    NO_ISR_ERROR 18 ; (isr_18) Machine Check
    NO_ISR_ERROR 19 ; (isr_19) SIMD FP Exception
    NO_ISR_ERROR 20 ; (isr_20) Virtualization Exception
    NO_ISR_ERROR 21 ; (isr_21) Control Protection Exception
    NO_ISR_ERROR 22 ; (isr_22) Reserved (unused)
    NO_ISR_ERROR 23 ; (isr_23) Reserved (unused)
    NO_ISR_ERROR 24 ; (isr_24) Reserved (unused)
    NO_ISR_ERROR 25 ; (isr_25) Reserved (unused)
    NO_ISR_ERROR 26 ; (isr_26) Reserved (unused)
    NO_ISR_ERROR 27 ; (isr_27) Reserved (unused)
    NO_ISR_ERROR 28 ; (isr_28) Reserved (unused)
    NO_ISR_ERROR 29 ; (isr_29) Reserved (unused)
    NO_ISR_ERROR 30 ; (isr_30) Reserved (unused)
    NO_ISR_ERROR 31 ; (isr_31) Reserved (unused)
