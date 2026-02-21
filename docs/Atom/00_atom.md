# Atom Runtime – Overview

The **Atom Runtime** is the foundational support layer of MoleculeOS.  
It provides a **minimal, deterministic, and fully freestanding** set of utilities that the kernel can rely on without depending on the C++ standard library, exceptions, dynamic memory, or any external runtime environment.

Atom is designed to be:

- **small** (only what the kernel truly needs)  
- **predictable** (no hidden allocations or side effects)  
- **compile‑time friendly** (`constexpr`, `noexcept`, header‑only)  
- **fully freestanding** (usable from the earliest boot stages)  
- **didactically structured** (clear modules, clear responsibilities)

Atom forms the lowest layer of the MoleculeOS architecture — everything above it (IDT, GDT, drivers, scheduler, memory manager) builds on its guarantees.

---

## Purpose of Atom

Atom exists to solve a simple but fundamental problem:

- A kernel cannot rely on the C++ standard library —  
- but it still needs basic, safe, reusable building blocks.

Atom provides exactly those building blocks:

- tiny containers  
- safe compile‑time utilities  
- minimal C‑runtime replacements  
- deterministic helper functions  
- clean abstractions for kernel subsystems  

All components are designed to be **zero‑overhead**, meaning the compiler can inline or eliminate them entirely.

---

## Structure of the Atom Runtime

Atom is divided into **clear, self‑contained modules**, each with a single responsibility.  
This keeps the codebase clean, predictable, and easy to extend.

### **1. Atom/C – Low‑Level C Runtime**
Contains minimal C‑style utilities required by the kernel:

- `memcpy`, `memset`, `memcmp`
- `strlen`, `strcmp`
- panic and debug helpers
- low‑level pointer utilities

These functions are implemented without libc and are safe for early boot stages.

---

### **2. Atom/CPP – Minimal C++ Runtime**
Provides tiny, STL‑free C++ abstractions:

- `Array<T, S>` — static container for fixed‑size tables  
- `span<T>` — non‑owning view into contiguous memory  
- `string_view` — lightweight string reference  
- `Optional<T>` (optional future module)  
- `print` / formatting utilities (kernel‑safe)

All components are:

- header‑only  
- `constexpr`  
- `noexcept`  
- freestanding‑compatible  

---

### **3. Atom/API – Shared Kernel Interfaces**
Defines small, stable interfaces used across kernel subsystems:

- type aliases  
- constants  
- shared enums  
- architecture‑specific definitions  
- forward declarations for kernel modules  

This keeps the kernel modular and avoids circular dependencies.

---

## Design Philosophy

Atom follows three core principles:

### **1. Minimalism**
Only what is needed — nothing more.  
Every function and type exists because the kernel uses it.

### **2. Determinism**
No dynamic allocation, no exceptions, no hidden behavior.  
Every byte and every instruction is predictable.

### **3. Didactic Clarity**
Atom is not just a runtime — it is a **teaching tool**.  
Each module is intentionally small, readable, and easy to understand.

---

## Example: How Atom Integrates Into the Kernel

A typical kernel subsystem uses Atom like this:

```cpp
    #include <CPP/Array.hpp>
    #include <Atom/C/memory.h>

    atom::Array<IDTEntry, 256> idt;
    memset(idt.begin(), 0, idt.size() * sizeof(IDTEntry));
```

This demonstrates:

- static containers  
- low‑level memory utilities  
- zero‑overhead abstractions  

All without relying on the standard library.

---

## Documentation References

This master document links to all detailed MoleculeOS documentation: 

- [C++ API](../Atom/CPP)
- [C API](../Atom/C)

---

## Summary

Atom is the **core runtime layer** of MoleculeOS.  
It provides minimal, deterministic, and freestanding‑safe utilities that replace the C and C++ standard libraries inside the kernel.

Atom ensures that:

- the kernel remains small and predictable  
- all abstractions are compile‑time friendly  
- no hidden allocations or runtime dependencies exist  
- the codebase stays modular and didactically clean  

It is the foundation upon which the entire MoleculeOS architecture is built.
