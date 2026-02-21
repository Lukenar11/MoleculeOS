# **Atom Runtime – `string.h` (C Runtime Layer)**

These functions replace the standard library in the freestanding kernel environment and <br>
provide the essential memory operations required by both the compiler and the kernel.

The MoleculeOS C runtime is intentionally **small**, **deterministic**, and **fully freestanding**.

---

## `memcpy` – Copy raw memory

### **Purpose**

`memcpy` copies exactly `size` bytes from `source_ptr` to `dest_ptr`.  
It is used by:

- the compiler (structure copies, return‑value optimization)
- the kernel (copying buffers, initializing tables)
- static initialization of objects

This implementation does **not** support overlapping memory regions — this is intentional for simplicity and performance.

### **Code**

``` c
    void* memcpy(void* dest_ptr, const void* sourche_ptr, const uint32_t size) {

        uint8_t* dest = (uint8_t*)dest_ptr;
        const uint8_t* sourche = (const uint8_t*)sourche_ptr;

        for (uint32_t i = 0; i < size; i++) 
            dest[i] = sourche[i];

        return dest_ptr;
    }
```

---

## `memmove` – Safe copy with overlap support

### **Purpose**

Unlike `memcpy`, `memmove` correctly handles overlapping memory regions.  
This is important for:

- shifting buffers  
- moving overlapping structures  
- kernel string operations  

If the destination lies inside the source range, the copy direction is reversed to avoid overwriting data.

### **Code**

``` c
    void* memmove(void* dest_ptr, const void* src_ptr, const uint32_t size) {

        uint8_t* dest = (uint8_t*)dest_ptr;
        const uint8_t* src = (const uint8_t*)src_ptr;

        if (dest < src) {

            // Copy forward
            for (uint32_t i = 0; i < size; i++)
                dest[i] = src[i];
        } else {

            // Copy backward (in case of overlap)
            for (uint32_t i = size; i > 0; i--)
                dest[i - 1] = src[i - 1];
        }

        return dest_ptr;
    }
```

---

## `memset` – Fill memory with a byte value

### **Purpose**

`memset` writes the same byte value into `size` bytes of memory.  
Used for:

- clearing buffers  
- initializing tables
- resetting kernel memory regions  

This is one of the most frequently used functions in low‑level code.

### **Code**

``` c
    void* memset(void* dest_ptr, int value, const uint32_t size) {

        uint8_t* dest = (uint8_t*)dest_ptr;
        for (uint32_t i = 0; i < size; i++)
            dest[i] = (uint8_t)value;

        return dest_ptr;
    }
```

---

## `memcmp` – Compare two memory regions

### **Purpose**

`memcmp` compares two memory blocks byte‑by‑byte.  
It returns:

- `< 0` if `a_ptr < b_ptr`
- `0` if equal
- `> 0` if `a_ptr > b_ptr`

Useful for:

- comparing signatures  
- checking if buffers match  
- validating structures  

### **Code**

```c
    int memcmp(const void* a_ptr, const void* b_ptr, const uint32_t size) {

        const uint8_t* a = (const uint8_t*)a_ptr;
        const uint8_t* b = (const uint8_t*)b_ptr;

        for (uint32_t i = 0; i < size; i++) 
            if (a[i] != b[i]) 
                return (int)a[i] - (int)b[i];
        return 0;
    }
```

---

## Header File: `string.h`

``` c
    #ifndef STRING_H
    #define STRING_H

    #include <stdint.h>

    #ifdef __cplusplus
        extern "C" {
    #endif

        void* memcpy(void* dest_ptr, const void* sourche_ptr, const uint32_t size);
        void* memmove(void* dest_ptr, const void* sourche_ptr, const uint32_t size);
        void* memset(void* dest_ptr, int value, const uint32_t size);
        int memcmp(const void* a_ptr, const void* b_ptr, const uint32_t size);
    #ifdef __cplusplus
        }
    #endif

    #endif  // STRING_H
```

---

# **Summary**

The MoleculeOS C runtime provides the minimal set of memory functions required by both the compiler and the kernel.  
These functions are:

- freestanding‑compatible
- deterministic
- simple and safe
- fully under kernel control
- optimized for early boot

They form the foundation for all higher‑level runtime components.
