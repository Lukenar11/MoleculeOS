# Interrupt Descriptor Table (IDT)

This document describes how MoleculeOS sets up and manages the **Interrupt Descriptor Table (IDT)** in 32‑bit protected mode. <br>
The IDT is a core CPU structure that defines how the processor handles **exceptions**, **hardware interrupts**, and **software interrupts**. <br>

A correct IDT setup is essential before enabling interrupts or executing any code that may trigger CPU exceptions.

---

## Purpose of the IDT

The IDT provides the CPU with:

- the **address** of each interrupt handler  
- the **segment selector** used to enter kernel mode  
- the **type** of interrupt gate  
- the **privilege level** required to trigger it  
- the **present bit**, indicating whether the entry is valid  

MoleculeOS uses the IDT to route all CPU exceptions (0–31) to a unified C++ handler via small assembly stubs.

---

## IDT Entry Structure

Each IDT entry is 8 bytes long and contains:

```text
   +-----------------------+
   | base_low   (16 bits)  | → lower part of handler address
   +-----------------------+
   | selector   (16 bits)  | → GDT code segment selector
   +-----------------------+
   | always_0   (8 bits)   | → must be zero
   +-----------------------+
   | flags      (8 bits)   | → type, privilege, present
   +-----------------------+
   | base_high  (16 bits)  | → upper part of handler address
   +-----------------------+
```

MoleculeOS defines this structure in C++ and marks it as `packed` to ensure the CPU‑required layout:

``` cpp
   /*
      "__attribute__((packed))" prevents the compiler from filling the structure
      with padding bytes. 
      The IDT entries must exactly match the layout defined by
      Intel, as the CPU reads the fields directly from memory.
      Any deviation (e.g., due to alignment)
      would result in incorrect handler addresses or an immediate triple fault.
      Therefore, "packed" is absolutely necessary here.
   */
```

---

MoleculeOS stores this in:

``` cpp
   // Interrupt Descriptor
   struct IdtEntry final {

      uint16_t base_low;
      uint16_t selector;
      uint8_t always_0;
      uint8_t flags;
      uint16_t base_high;

      // _init_
      inline constexpr void set_gate
         (const uint32_t base, const uint16_t selector, const uint8_t flags) {

         base_low = base & MASK;
         base_high = (base >> 16) & MASK;
         this->selector = selector;
         this->always_0 = 0;
         this->flags = flags;
      }
   } __attribute__((packed));
```

---

## IDT Pointer (IDTR Descriptor)

The CPU loads the IDT using a special descriptor:

```text
   +-----------------------+
   | limit (16 bits)       | → size of IDT - 1
   +-----------------------+
   | base  (32 bits)       | → address of IDT
   +-----------------------+
```

---

MoleculeOS stores this in:

``` cpp
   // IDT-Descriptor Pointer (needed for "lidt[...]")
   struct IdtPtr final {

      uint16_t limit;
      uint32_t base;
   } __attribute__((packed));
```

This structure is also packed.

---

## Memory Layout of the IDT in MoleculeOS

```text
   +-------------------------------+
   | IDT Entry 0  → isr_0          |
   +-------------------------------+
   | IDT Entry 1  → isr_1          |
   +-------------------------------+
   | ...                           |
   +-------------------------------+
   | IDT Entry 31 → isr_31         |
   +-------------------------------+
   | IDT Entry 32–255 (unused)     |
   +-------------------------------+
```

Only the first 32 entries are initialized at this stage.

---

## Initialization Process

The IDT class in MoleculeOS performs the following steps:

1. **Clear all 256 entries**  
   Every entry is initialized with zeros to avoid undefined behavior.

2. **Apply the initialization table**  
   A static table maps exception numbers (0–31) to their corresponding ASM stubs.

3. **Set each entry using `set_gate()`**  
   This function writes the handler address, selector, and flags.

4. **Load the IDT using `LoadIDT()`**  
   The IDTR register is updated with the address and size of the IDT.

---

## Full Initialization Code (C++ Constructor)

``` cpp
   // _construct_
   kernel::idt::IDT::IDT() {

      // fill IDT-Descriptor
      idt_ptr.limit = (sizeof(IdtEntry) * ENTRYS) - 1;
      idt_ptr.base = uint32_t(&idt);

      // Clear table
      for (int i = 0; i < ENTRYS; i++)
         idt[i].set_gate(0, 0, 0);

      // _build_ IDT
      for (auto& entry : idt_init_table)
         idt[entry.index].set_gate(uint32_t(entry.handler), 0x08, 0x8E);

      LoadIDT(uint32_t(&idt_ptr));
   }
```

---

## Code Example: Setting an IDT Entry

```cpp
   idt[entry.index].set_gate(uint32_t(entry.handler), 0x08, 0x8E);
```

---

## Why `0x8E`?

`0x8E` represents:

- **1** → Present  
- **00** → DPL = Ring 0  
- **1110** → 32‑bit interrupt gate  

This is the standard configuration for kernel exceptions.

---

## Summary

The IDT in MoleculeOS:

- defines how the CPU handles exceptions  
- maps each exception to a dedicated ASM stub  
- ensures a consistent transition into C++ code  
- is loaded into the CPU using the `lidt` instruction  
- forms the foundation for IRQ handling and system stability  

A correct IDT setup is mandatory before enabling interrupts or executing complex kernel code.
