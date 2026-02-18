#pragma once

#include <stdint.h>

/*
    "__attribute__((packed))" prevents the compiler from filling the structure
    with padding bytes. 
    The IDT entries must exactly match the layout defined by
    Intel, as the CPU reads the fields directly from memory.
    Any deviation (e.g., due to alignment)
    would result in incorrect handler addresses or an immediate triple fault.
    Therefore, "packed" is absolutely necessary here.
*/

namespace kernel::idt {

    static constexpr int ENTRYS = 256;
    static constexpr int MASK = 0xFFFF;

    // IDT-Descriptor => IDTR-Register (LoadIDT.asm)
    extern "C" void LoadIDT(uint32_t);

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

    // Mapping IDT Index -> Handler Merchants
    struct IdtInitEntry final {

        int index;
        void (*handler)();
    };

    // IDT-Descriptor Pointer (needed for "lidt[...]")
    struct IdtPtr final {

        uint16_t limit;
        uint32_t base;
    } __attribute__((packed));

    class IDT final {

        private:
            IdtEntry idt[256];
            IdtPtr idt_ptr;

        public:
            inline IdtEntry& operator[]
                (int index) {return idt[index];}

            inline const IdtEntry& operator[]
                (int index) const {return idt[index];}

            IDT();
            ~IDT() = default;
    };
} // namespache kernel
