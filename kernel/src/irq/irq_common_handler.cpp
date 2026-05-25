/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
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
void irq_common_handler(Register_Dump* reg_dump)
{
    const uint8_t interrupt_vector = reg_dump->interrupt_number;
    const uint8_t eoi = 0x20;

    const uint8_t min_interrupt_vector = 0x28;
    const uint8_t max_interrupt_vector = 0x2F;

    if ((interrupt_vector >= min_interrupt_vector) && 
        (interrupt_vector <= max_interrupt_vector)) [[likely]] {
        const uint16_t slave_pic_command_port = 0xA0;
        outb(slave_pic_command_port, eoi);
    }

    const uint16_t master_pic_command_port = 0x20;
    outb(master_pic_command_port, eoi);
}
