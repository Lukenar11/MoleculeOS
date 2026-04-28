#include "IDT/IRQ/irq_common_handler.hpp"

extern "C"
void irq_common_handler(RegisterDump* reg_dump) {
    HaltSystem();
}
