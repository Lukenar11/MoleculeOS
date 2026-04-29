#include "IDT/IRQ/irq_common_handler.hpp"

void register_irq_handler(int irq, void (*handler)(RegisterDump*)) 
{
    if (irq < 0 || irq > 15) return;
    irq_handlers[irq] = handler;
}

extern "C"
void irq_common_handler(RegisterDump* reg_dump)
{
    int irq = reg_dump->interrupt_number - 32;

    if (irq >= 0 && irq < 16 && irq_handlers[irq] != nullptr)
        irq_handlers[irq](reg_dump);

    if (irq >= 8)
        kernel::utils::outb(0xA0, 0x20);
    kernel::utils::outb(0x20, 0x20);
}
