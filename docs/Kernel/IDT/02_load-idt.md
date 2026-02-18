# Loading the Interrupt Descriptor Table (IDT)

This document explains how MoleculeOS loads the IDT into the CPU using the `lidt` instruction. <br>
The IDT must be loaded before any interrupt or exception can be handled safely. <br>

---

## Purpose of `LoadIDT`

The function `LoadIDT(uint32_t idt_ptr_address)` is implemented in assembly and performs exactly one task:

- Load the IDT descriptor into the CPU’s **IDTR register**.

This activates the IDT defined by the kernel.

---

## IDTR Register

The IDTR register contains:

``` text
    +-----------------------+
    | limit (16 bits)       | → size of IDT - 1
    +-----------------------+
    | base  (32 bits)       | → address of IDT
    +-----------------------+
```

The CPU uses this information to locate the IDT.

---

## Code: LoadIDT.asm

```asm
    global LoadIDT

    section .text
        LoadIDT:
            mov eax, [esp+4]    ; Stack-Address + 4 Bytes => Accumulator
            lidt [eax]          ; _init_ IDT
            ret
```

---

## How It Works

### 1. The C++ code calls:

```cpp
    // IDT-Descriptor => IDTR-Register (LoadIDT.asm)
    extern "C" void LoadIDT(uint32_t);
```

---

### 2. The assembly function receives the argument on the stack:

- `[esp+4]` → address of IDT descriptor

---

### 3. The function loads the IDTR:

``` asm
    ; _init_ IDT
    lidt [eax]
```

This tells the CPU:

- where the IDT is located  
- how large it is  

---

### 4. The function returns to C++.

---

## Why a Separate Assembly File?

Even though `lidt` is a single instruction, it must be executed in assembly because:

- C++ cannot emit `lidt` directly  
- inline assembly is compiler‑specific and less portable  
- separating architecture‑specific code improves clarity  
- MoleculeOS keeps ABI‑critical instructions isolated  

This design keeps the kernel clean and modular.

---

## Summary

`LoadIDT` is a minimal but essential part of MoleculeOS:

- it loads the IDT descriptor into the CPU  
- it activates the interrupt system  
- it is required before enabling interrupts  
- it is implemented in a clean, isolated assembly file  

Without this step, the CPU would have no way to handle exceptions or interrupts.
