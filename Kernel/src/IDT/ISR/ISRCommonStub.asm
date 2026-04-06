global ISRCommonStub
extern isr_common_handler

section .text

global ISRCommonStub
extern isr_common_handler

section .text
    
    ISRCommonStub:
        ; General Purpose Register => Stack
        push eax
        push ecx
        push edx
        push ebx
        push ebp
        push esi
        push edi
    
        ; Segmentregister => Stack
        push ds
        push es
        push fs
        push gs
    
        ; Kernel Data Segment setzen
        mov ax, 0x10
        mov ds, ax
        mov es, ax
        mov fs, ax
        mov gs, ax
    
        ; Pointer auf RegisterDump übergeben
        push esp
        call isr_common_handler
        add esp, 4
    
        ; Segmentregister wiederherstellen
        pop gs
        pop fs
        pop es
        pop ds
    
        ; General Purpose Register wiederherstellen
        pop edi
        pop esi
        pop ebp
        pop ebx
        pop edx
        pop ecx
        pop eax
    
        ; interrupt_number + error_code entfernen
        add esp, 8
    
        iretd
    