# Fixed‑Width Integer Types (`stdint.h`)

MoleculeOS provides a minimal, freestanding implementation of the C99 fixed‑width integer types. <br>
These types guarantee **exact bit sizes**, **predictable behavior**, and **architecture‑independent integer widths**, which are essential for low‑level kernel development. <br>

The header defines only the core integer types required for early kernel code, hardware access, and freestanding C/C++ environments.

---

## Why Fixed‑Width Types Are Needed

Standard C/C++ integer types (`char`, `short`, `int`, `long`) do **not** guarantee fixed sizes. <br>

Their bit width depends on:

- Compiler  
- CPU architecture  
- ABI (Application Binary Interface)  

---

This makes them unsuitable for:

- hardware registers  
- memory‑mapped I/O  
- bitfields  
- binary protocols  
- kernel structures  

Fixed‑width types solve this by guaranteeing exact sizes.

---

## How Standard C Types Behave

### Signed Types
- `char` → **at least** 8 bits (may be signed or unsigned depending on compiler)
- `short` → **at least** 16 bits
- `int` → **at least** 16 bits (commonly 32 bits on modern systems)
- `long` → 32 or 64 bits depending on platform

### Unsigned Types
- `unsigned char` → 8 bits  
- `unsigned short` → 16 bits  
- `unsigned int` → 16 or 32 bits  
- `unsigned long` → 32 or 64 bits  

None of these are guaranteed to have a fixed width across platforms.

---

## How Fixed‑Width Types Work (complete `stdint.h` Code)

Your header defines:

``` c
    #ifndef _STDINT_H
    #define _STDINT_H

    #ifdef __cplusplus
        extern "C" {
    #endif

        typedef signed char int8_t;
        typedef signed short int16_t;
        typedef signed int int32_t;

        typedef unsigned char uint8_t;
        typedef unsigned short uint16_t;
        typedef unsigned int uint32_t;
    #ifdef __cplusplus
        }
    #endif

    #endif  // _STDINT_H
```

### Why these mappings are correct

- `signed char` is **always** 8 bits → perfect for `int8_t`
- `signed short` is **always** 16 bits → perfect for `int16_t`
- `signed int` is **guaranteed** to be ≥16 bits, but on all x86 compilers it is **exactly 32 bits** → correct for `int32_t`

Same logic applies to the unsigned versions.

---

## Bit Width Guarantees

| Type      | Bits | Range                       |
|-----------|------|-----------------------------|
| `int8_t`  | 8    | −128 … +127 |
| `int16_t` | 16   | −32,768 … +32,767 |
| `int32_t` | 32   | −2,147,483,648 … +2,147,483,647 |
| `uint8_t` | 8    | 0 … 255 |
| `uint16_t`| 16   | 0 … 65,535 |
| `uint32_t`| 32   | 0 … 4,294,967,295 |

These ranges are mathematically guaranteed by the bit width.

---

## Summary 

- defines fixed bit widths for all core integer types
- solves the problem of variable standard types (`int`, `short`, `long`)
- guarantees deterministic behavior in hardware and kernel code
- completely freestanding and without standard library
- compatible with C and C++ through `extern "C"`
