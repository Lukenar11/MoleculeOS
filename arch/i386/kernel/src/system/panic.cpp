#include "system/panic.hpp"

namespace kernel::system
{
    void panic(const char* message) noexcept {
        const uint32_t reserved_custom_panic_idt_interrupt = 15;

        isr::exception_names[
            reserved_custom_panic_idt_interrupt
        ] = const_cast<char*>(message);
        
        trigger_interrupt(reserved_custom_panic_idt_interrupt);
    }
} // namespace kernel::system
