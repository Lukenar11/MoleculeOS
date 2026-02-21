# Atom Runtime – `atom::Array<T, S>`

This chapter describes the implementation and purpose of the **`atom::Array`** container within the MoleculeOS runtime. <br>
The container provides a **static, fully freestanding-capable array structure** <br>
that works without the STL, exceptions, or the C++ runtime and can be fully optimized by the compiler.

---

## Purpose of atom::Array

The kernel requires a **reliable, minimal, and fully deterministic** data structure that:

- works without dynamic memory management
- operates without exceptions and the standard library
- is fully `constexpr` and `noexcept`
- can be completely optimized away by the compiler
- has a clear, simple API

`atom::Array<T, S>` meets exactly these requirements and forms the basis for further runtime components such as tables, <br>
handler mappings, or static kernel data.

---

## Design Principles

The container follows four central MoleculeOS guidelines:

### 1. Minimalism
Only functions that are actually needed in the kernel are implemented:

- `operator[]`
- `size()`
- `begin()`, `end()`
- `front()`, `back()`
- `fill()`

Everything else (e.g., **exceptions**, **debug iterators**, **reverse iterators**, **allocator support**) has been deliberately omitted.

---

### **2. Compile-Time Safety**

A `static_assert` ensures that arrays are never created with a size of **0**:

```cpp
    static_assert(S > 0, "Array size must be greater than zero");
```

Errors are caught **early** — long before the kernel is even compiled.

---

### **3. 100% Dead Code Elimination**

All methods are:

- `constexpr`
- `inline`
- `noexcept`
- `const`
- header-only (template)

This allows the compiler to:

- completely remove unused functions
- fully inline used functions
- optimize loops
- reduce memory accesses

The container generates **not a single byte** of unnecessary code.

---

### 4. Freestanding Compatibility

The container uses only:

- `stdint.h`
- `string.h`

and no STL headers. <br>
It is therefore fully freestanding-compatible and ideal for kernel code. <br>

---

## Memory Layout of the Array

`atom::Array<T, S>` stores its elements **inline**, without pointers or heap allocation:

``` text
    +---------------------------+
    | T[0]                      |
    +---------------------------+
    | T[1]                      |
    +---------------------------+
    | ...                       |
    +---------------------------+
    | T[S-1]                    |
    +---------------------------+
```

This guarantees:

- constant access time
- no fragmentation
- no hidden dependencies
- complete control over the layout  


---

## `atom::Array<T, S>` – Static Kernel Array

This chapter describes the implementation of the `atom::Array` container within the MoleculeOS runtime. <br>
The container provides a **static, fully freestanding-capable array** that works without STL, exceptions, or dynamic memory management. <br>

---

## Purpose of the Container

The kernel requires a **deterministic, minimalist, and fully optimizable** data structure that:

- works without heap allocation
- does not rely on the standard library
- guarantees a fixed memory layout
- is entirely `constexpr` and `noexcept`
- can be completely optimized away by the compiler

`atom::Array<T, S>` meets exactly these requirements and forms the basis for static kernel tables like the IDT.

---

## Design Principles

- **Minimalism:** Only functions actually needed by the kernel
- **Compile-Time Safety:** `static_assert` prevents invalid sizes
- **Zero-Overhead:** Everything is `constexpr`, `inline`, and `noexcept`
- **Freestanding:** No dependencies on `std::array` or other STL components

---

## Method Overview

- `operator[](index)`
Directly accesses the element at position `index`.
No bounds checking — deliberately for kernel performance.


- `size()`
Returns the static size `S`.
Since `S` is a template parameter, evaluation is done entirely at compile time.


- `begin()` / `end()`
Return pointers to the start and end of the array, respectively.
Allow range-based loops:

    ```cpp
        for (auto& e : array) { ... }
    ```

- `front()`
Returns the first element of the array.
Frequently used for table initialization.

- `back()`
Returns the last element of the array.
Useful for static structures like handler tables.

- `fill(value)`
Sets all elements of the array to the same value.
For example, used to clear tables before initialization.

---

## Variadic Constructor

To support clean and expressive initialization of static kernel tables, <br>
atom::Array<T, S> provides a variadic constructor that allows passing exactly `S` values directly:

``` cpp
    template<class... Args>
    constexpr Array(Args... args) noexcept;
```

Rules:

The number of arguments must match the array size `S`  
Enforced at compile time:

``` cpp
    static_assert(
        sizeof...(Args) == S,
        "Number of constructor arguments must match array size"
    );
```

Each argument is converted to `T` using `static_cast<T>`
No dynamic memory, no exceptions, fully freestanding
Perfect for static kernel structures such as the IDT, GDT, or handler tables

---

Example
``` cpp
    atom::Array<int, 10> array(0, 1, 2, 3, 4, 5, 6, 7, 8, 9);
```

This constructor allows the compiler to fully optimize the initialization and guarantees a deterministic layout.

``` cpp
    // _construct_ (for "atom::Array<...> name(...);")
    template<class... Args>
    constexpr Array(Args... args) noexcept : data {static_cast<T>(args)...} {
    
        static_assert(
            sizeof...(Args) == S,
            "Number of constructor arguments must match array size"
        );
    }
```

---

## Complete code

``` cpp
    #pragma once
    
    #include <stdint.h>
    #include <C/string.h>
    
    namespace atom {
    
        template<class T, uint32_t S>
        class Array final {
        
            private:
                static_assert(S > 0, "Array size must be greater than zero");
                T data[S];
            
            public:
                // Data access
                inline constexpr T& operator[]
                    (uint32_t index) noexcept {return data[index];}
    
                inline constexpr const T& operator[]
                    (uint32_t index) const noexcept {return data[index];}
    
                // _get_ size
                inline constexpr uint32_t size() const noexcept {return S;}
    
                // Iterators
                inline constexpr T* begin() noexcept {return data;}
                inline constexpr T* end() noexcept {return data + S;}
    
                inline constexpr const T* begin() const noexcept {return data;}
                inline constexpr const T* end() const noexcept {return data + S;}
    
                // Element access
                inline constexpr T& front() noexcept {return data[0];}
                inline constexpr T& back() noexcept {return data[S - 1];}
    
                inline constexpr const T& front() const noexcept {return data[0];}
                inline constexpr const T& back() const noexcept {return data[S - 1];}
    
                // fill with value
                constexpr void fill(const T& value) noexcept {
                
                    for (uint32_t i = 0; i < S; ++i)
                        data[i] = value;
                }
        
                // _construct_ (for "atom::Array<...> name(...);")
                template<class... Args>
                constexpr Array(Args... args) noexcept : data {static_cast<T>(args)...} {
                
                    static_assert(
                        sizeof...(Args) == S,
                        "Number of constructor arguments must match array size"
                    );
                }
    
                Array(const Array&) = delete;
                Array& operator=(const Array&) = delete;
    
                constexpr Array() noexcept = default;
                ~Array() noexcept = default;
        };
    } // namespace atom
```

---

## Example: Usage in the IDT Subsystem

The container is used, for example, for the Interrupt Descriptor Table:

```cpp
    atom::Array<IDTEntry, 256> idt;
```

This guarantees:

- constant layout
- no dynamic allocation
- no dependency on `std::array`
- full control over the structure

---

## Why not `std::array`?

The standard library is unsuitable for kernel code:

- requires exceptions
- requires RTTI
- requires a C++ runtime
- contains debug iterators
- is not freestanding
- generates unnecessary code

`atom::Array`, on the other hand, is:

- minimal
- deterministic
- fully optimizable
- OS-compatible
- didactically clear

---

## Summary

`atom::Array<T, S>` is a minimal, fully freestanding-capable container of the MoleculeOS runtime. <br>
It replaces std::array in the kernel context and guarantees a deterministic, static memory layout, <br>
without exceptions, without dynamic allocation, and without dependence on the standard library.

The container is:

- **constexpr-capable**, **noexcept-safe**, and **header-only**, allowing the compiler to fully optimize it or even completely eliminate it
- **compile-time safe** through `static_assert`, so invalid array sizes are detected early
- deliberately minimalist, with only the methods actually needed in the kernel **(`operator[]`, `size()`, `begin() / end()`, `front() / back()`, `fill()`)**
- ideal for static kernel structures such as the IDT, GDT, or handler tables

Through its simple layout and strict freestanding compatibility, <br>
`atom::Array` forms a central component of the MoleculeOS runtime and enables clear, efficient, and fully controllable memory management in the kernel.
