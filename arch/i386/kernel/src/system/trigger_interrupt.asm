global trigger_interrupt

section .text

trigger_interrupt:
    mov eax, [esp+4]
    cmp eax, 255
    ja  .done
    jmp [interrupt_table + eax*4]

.done:
    ret

interrupt_table:
%assign i 0
%rep 256
    dd int_%+i
%assign i i+1
%endrep

%assign i 0
%rep 256
int_%+i:
    int i
    ret
%assign i i+1
%endrep
