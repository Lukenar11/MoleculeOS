/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    C interface for the general IRQ handler, 
    which is called by "irq_common_stub".

    All hardware interrupt stubs (irq_0–irq_15) 
    pass their register state to this function,
    which performs uniform processing before the required
    End-of-Interrupt (EOI) signal is sent to the PIC.

NOTES:
    Since this function is called by an assembly routine, 
    it is declared as "extern "C"" to ensure compatibility.
*/

#include "irq/irq_common_handler.hpp"

namespace kernel::irq
{
    extern "C"
    void irq_common_handler(Register_Dump* reg_dump) {
        const uint8_t end_of_interrupt     = 0x20;
        const uint8_t min_interrupt_vector = 0x28;
        const uint8_t max_interrupt_vector = 0x2F;
        const uint8_t max_irq_event_number = 15;

        const uint8_t interrupt_vector = reg_dump->interrupt_number;
        const uint8_t irq_event        = interrupt_vector - end_of_interrupt;

        if ((irq_event <= max_irq_event_number) && 
            (kernel::irq::irq_handler_table[irq_event].handler != nullptr)) 
            kernel::irq::irq_handler_table[irq_event].handler(reg_dump);
    
        if ((interrupt_vector >= min_interrupt_vector) && 
            (interrupt_vector <= max_interrupt_vector)) {
            const uint16_t slave_pic_command_port = 0xA0;
            runtime::byte_output(slave_pic_command_port, end_of_interrupt);
        }
    
        const uint16_t master_pic_command_port = 0x20;
        runtime::byte_output(master_pic_command_port, end_of_interrupt);
    }
} // namespace kernel::irq
