# MoleculeOS – CPU Exception Stubs

This document describes how MoleculeOS handles **CPU exceptions (0–31)** in 32‑bit protected mode. <br> 
Each exception is routed through a small assembly stub before reaching the unified C++ handler. <br>

---

## Purpose of the Exception Stubs

The CPU cannot directly call C++ functions.  
Therefore, MoleculeOS uses small assembly routines (ISR stubs) that:

- prepare the stack in a predictable format  
- pass the interrupt number  
- pass the error code (real or synthetic)  
- call the shared C++ handler  
- restore the CPU state and return safely  

This ensures that all exceptions — regardless of their type — arrive in the kernel in a **uniform, structured way**.

---

## CPU Behavior During an Exception

When an exception occurs, the CPU automatically pushes several registers onto the stack:

``` text
    EIP
    CS
    EFLAGS
    (optional) ESP
    (optional) SS
    (optional) ERROR CODE
```

Whether an error code is pushed depends on the exception type.

---

## Stack Layout During an Exception

``` text
    Stack layout during an exception (before MoleculeOS pushes anything):

        +-------------------+
        | EFLAGS            |
        +-------------------+
        | CS                |
        +-------------------+
        | EIP               |
        +-------------------+
        | (Error Code?)     | ← only for some exceptions
        +-------------------+
```

    After the ISR stub pushes interrupt number + error code:

``` text
    +-------------------+
    | interrupt number  |
    +-------------------+
    | error code        |
    +-------------------+
    | return address    | ← from "call isr_common_handler"
    +-------------------+
    | EIP               |
    +-------------------+
    | CS                |
    +-------------------+
    | EFLAGS            |
    +-------------------+
``` 
## Exceptions With and Without Error Codes

Some exceptions push an error code, others do not.

| Exception | Number | Error Code |
|-----------|--------|------------|
| Divide Error | 0 | ❌ |
| Debug | 1 | ❌ |
| NMI | 2 | ❌ |
| Breakpoint | 3 | ❌ |
| Overflow | 4 | ❌ |
| Bound Range | 5 | ❌ |
| Invalid Opcode | 6 | ❌ |
| Device Not Available | 7 | ❌ |
| Double Fault | 8 | ✔ |
| Invalid TSS | 10 | ✔ |
| Segment Not Present | 11 | ✔ |
| Stack Fault | 12 | ✔ |
| General Protection Fault | 13 | ✔ |
| Page Fault | 14 | ✔ |
| Alignment Check | 17 | ✔ |

All other exceptions do **not** push an error code.

---

## Why Two Macros?

To avoid writing 32 nearly identical stubs, MoleculeOS uses two macros:

### `NO_ISR_ERROR`
For exceptions **without** an error code.  
A synthetic error code (`0`) is pushed to maintain a consistent stack layout.

### `ISR_ERROR`
For exceptions **with** an error code.  
The CPU already pushed a real error code.  
(Currently still replaced with a placeholder — TODO.)

Even if both macros look identical now, <br>
they represent **two different architectural cases** and will diverge once real error codes are forwarded.

---

## Stack Layout Before Calling the C++ Handler

After the stub pushes its values, the stack looks like this:

```
[esp+0] → interrupt number
[esp+4] → error code (real or synthetic)
[esp+8] → return address (from call)
[esp+12] → EIP (CPU)
[esp+16] → CS
[esp+20] → EFLAGS
```

The C++ handler receives:

```cpp
    void isr_common_handler(uint32_t interrupt_number, uint32_t error_code);
```

After returning, the stub removes the two arguments:

```
add esp, 8
```

`iretd` restores the CPU state and resumes execution.

---

## Global Export of ISR Symbols

All 32 ISR entry points are exported in a single, compact declaration:

```asm
global isr_0, isr_1, isr_2, isr_3, isr_4, isr_5, isr_6, isr_7, \
       isr_8, isr_9, isr_10, isr_11, isr_12, isr_13, isr_14, isr_15, \
       isr_16, isr_17, isr_18, isr_19, isr_20, isr_21, isr_22, isr_23, \
       isr_24, isr_25, isr_26, isr_27, isr_28, isr_29, isr_30, isr_31
```

This keeps the file clean and avoids redundant declarations.

---

## Code Example: Exception Stub Without Error Code

``` asm
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
```

---

## Code Example: Exception Stub With Error Code

``` asm
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
```

---

## TODO: Forward Real Error Codes

Currently, exceptions that generate an error code still push a placeholder (`0`). <br>  
In a future revision, MoleculeOS will extract and forward the real CPU error code. <br>

---

## Summary

The ISR stubs in MoleculeOS:

- unify exception handling  
- normalize stack layout  
- bridge ASM and C++  
- prepare the kernel for robust error reporting  
- form the foundation for IRQ handling and later subsystems  

This system ensures that all CPU exceptions are processed safely, predictably, and consistently.
