# runtime – Overview

The **runtime** is the foundational support layer of MoleculeOS. <br>
It provides a **minimal, deterministic, and fully freestanding** set of utilities that the kernel can rely <br>
on without depending on the C++ standard library, exceptions, dynamic memory, or any external runtime environment. <br>

The runtime is designed to be:

- **small** (only what the kernel truly needs)  
- **predictable** (no hidden allocations or side effects)  
- **compile‑time friendly** (`constexpr`, `noexcept`, header‑only)   
- **didactically structured** (clear modules, clear responsibilities)
---

## Purpose of the runtime

The runtime exists to solve a simple but fundamental problem:

- A kernel cannot rely on the C++ standard library —  
- but it still needs basic, safe, reusable building blocks.

the runtime provides exactly those building blocks:

- tiny containers  
- safe compile‑time utilities  
- minimal C‑runtime replacements  
- deterministic helper functions  
- clean abstractions for kernel subsystems  

All components are designed to be **zero‑overhead**, meaning the compiler can inline or eliminate them entirely.

---

## Structure of the runtime

The runtime is divided into **clear, self‑contained modules**, each with a single responsibility.  
This keeps the codebase clean, predictable, and easy to extend.

### **1. Runtime/C – Low‑Level C Runtime**
Contains minimal C‑style utilities required by the kernel:

- `memcpy`, `memmove`, `memset`, `memcmp`
- `int8_t`, `int16_t`, `int32_t`, , `uint8_t`, `uint16_t`, `uint32_t`,

These functions are implemented without **libc** and are safe for early boot stages.

---

### **2. Runtime/CPP – Minimal C++ Runtime**
Provides tiny, STL‑free C++ abstractions:

- `Array<T, S>` — static container for fixed‑size tables  

All components are:

- header‑only  
- `constexpr`  
- `noexcept`  
- freestanding‑compatible  

---

## Design Philosophy

The runtime follows three core principles:

### **1. Minimalism**
Only what is needed — nothing more.  
Every function and type exists because the kernel uses it.

### **2. Determinism**
No dynamic allocation, no exceptions, no hidden behavior.  
Every byte and every instruction is predictable.

### **3. Didactic Clarity**
This is not just a runtime — it is a **teaching tool**.  
Each module is intentionally small, readable, and easy to understand.

---

## Documentation References

This master document links to all detailed MoleculeOS documentation: 

- [C++ API](../Runtime/CPP)
- [C API](../Runtime/C)

---

## Summary

The runtime is the **core runtime layer** of MoleculeOS.  
It provides minimal, deterministic, and freestanding‑safe utilities that replace the C and C++ standard libraries inside the kernel.

the runtime ensures that:

- the kernel remains small and predictable  
- all abstractions are compile‑time friendly  
- no hidden allocations or runtime dependencies exist  
- the codebase stays modular and didactically clean  

It is the foundation upon which the entire MoleculeOS architecture is built.
