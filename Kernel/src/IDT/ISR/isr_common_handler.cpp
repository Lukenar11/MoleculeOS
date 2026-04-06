#include "IDT/ISR/isr_common_handler.hpp"

extern "C" void isr_common_handler(RegisterDump* r) {

    runtime::console.reset();
    runtime::console.set_char_colors(
        drivers::vga::VGAColors::RED,
        drivers::vga::VGAColors::BLACK
    );

    runtime::console.put_string("\n[EXCEPTION] #");
    runtime::console.put_hex(r->interrupt_number);

    runtime::console.put_string("\nERR: ");
    runtime::console.put_hex(r->error_code);

    runtime::console.put_string("\nEIP: ");
    runtime::console.put_hex(r->eip);

    runtime::console.put_string("\nEFLAGS: ");
    runtime::console.put_hex(r->eflags);

    runtime::console.put_string("\nEAX: ");
    runtime::console.put_hex(r->eax);

    runtime::console.put_string("\nEBX: ");
    runtime::console.put_hex(r->ebx);

    runtime::console.put_string("\nECX: ");
    runtime::console.put_hex(r->ecx);

    runtime::console.put_string("\nEDX: ");
    runtime::console.put_hex(r->edx);

    runtime::console.put_string("\nESI: ");
    runtime::console.put_hex(r->esi);

    runtime::console.put_string("\nEDI: ");
    runtime::console.put_hex(r->edi);

    runtime::console.put_string("\nEBP: ");
    runtime::console.put_hex(r->ebp);

    while (true)
        __asm__ volatile("cli \n hlt");
}
