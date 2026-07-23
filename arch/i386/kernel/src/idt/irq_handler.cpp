/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    C++ interface for the general 'IRQ handler', 
    which is called by 'irq_stub'.

    All hardware interrupt stubs ('irq_0'...'irq_15') 
    pass their register state to this function, 
    which performs uniform processing before the required
    End-of-Interrupt (EOI) signal is sent to the PIC.

NOTES:
*/

#include "irq_handler.hpp"

namespace kernel::idt::irq
{
    extern "C"
    void irq_handler(Registers* reg_dump) {
        const uint8_t interrupt_vector = reg_dump->interrupt_number;
        const uint8_t irq_event        = interrupt_vector - EOF;

        if ((irq_event <= MAX_IRQ_EVENT_NUM) && 
            (irq_handler_table[irq_event].handler != nullptr)) 
            irq_handler_table[irq_event].handler(
                reinterpret_cast<Registers*>(reg_dump));
    
        if ((interrupt_vector >= MIN_INTERRUPT_VEC) && 
            (interrupt_vector <= MAX_INTERRUPT_VEC))
            runtime::byte_output(SLAVE_PIC_CMD_PORT, EOF);
    
        runtime::byte_output(MASTER_PIC_CMD_PORT, EOF);
    }
} // namespace kernel::irq
