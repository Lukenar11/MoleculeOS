/*
    LICENSE:
        Copyright (c) 2026 Lukenar11 (Luke Matthes)
        MIT License
        https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

    DESCRIPTION:
        C interface for the general IRQ handler, which is called by "irq_common_stub".

        All hardware interruptstubs (irq_0–irq_15) 
        pass their register state to this function,
        which performs uniform processing before the required
        End-of-Interrupt (EOI) signal is sent to the PIC.

        NOTES:
            Since this function is called by an assembly routine, 
            it is declared as "extern "C"" to ensure compatibility.
*/

#include "irq/irq_common_handler.hpp"

extern "C"
void irq_common_handler(RegisterDump* reg_dump)
{
    const uint8_t interrupt = reg_dump->interrupt_number;
    const uint8_t max_interrupt_size = 40;
    if (interrupt >= max_interrupt_size) {
        outb(0xA0, 0x20);
    }

    outb(0x20, 0x20);
}
