global ISRCommonStub
extern isr_common_handler

section .text
    
    ISRCommonStub:
        push eax
        push ebx
        push ecx
        push edx
        push esi
        push edi
        push ebp
    
        mov edi, esp
        call isr_common_handler
    
        pop ebx
        pop ecx
        pop edx
        pop esi
        pop edi
        pop ebp
        pop eax
    
        mov esp, 16
        iretd
    