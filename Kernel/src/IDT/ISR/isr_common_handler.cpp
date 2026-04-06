#include "IDT/ISR/isr_common_handler.hpp"

extern "C" void isr_common_handler(RegisterDump* red_dump) {

    runtime::console.reset();
    runtime::console.set_char_colors(
        drivers::vga::VGAColors::RED,
        drivers::vga::VGAColors::BLACK
    );

    runtime::console.put_string(">>>>>>>>>> !KERNEL PANIC! <<<<<<<<<<\n");

    runtime::console.put_string("\nException: ");
    runtime::console.put_string(exception_names[red_dump->interrupt_number]);
    runtime::console.put_string(" (");
    runtime::console.put_hex(red_dump->interrupt_number);
    runtime::console.put_string(")");

    runtime::console.put_string("\n\nCPU State:");
    runtime::console.put_string("\n  EIP: "); runtime::console.put_hex(red_dump->eip); 
    runtime::console.put_string("\n  EFLAGS: "); runtime::console.put_hex(red_dump->eflags); 

    runtime::console.put_string("\n\nGeneral Registers:");
    runtime::console.put_string("\n  EAX: "); runtime::console.put_hex(red_dump->eax);
    runtime::console.put_string("\n  EBX: "); runtime::console.put_hex(red_dump->ebx);
    runtime::console.put_string("\n  ECX: "); runtime::console.put_hex(red_dump->ecx);
    runtime::console.put_string("\n  EDX: "); runtime::console.put_hex(red_dump->edx); 

    runtime::console.put_string("\n  ESI: "); runtime::console.put_hex(red_dump->esi);
    runtime::console.put_string("\n  EDI: "); runtime::console.put_hex(red_dump->edi);
    runtime::console.put_string("\n  EBP: "); runtime::console.put_hex(red_dump->ebp); 

    runtime::console.put_string("\n\nSegment Registers:");
    runtime::console.put_string("\n  DS: "); runtime::console.put_hex(red_dump->ds);
    runtime::console.put_string("\n  ES: "); runtime::console.put_hex(red_dump->es);
    runtime::console.put_string("\n  FS: "); runtime::console.put_hex(red_dump->fs);
    runtime::console.put_string("\n  GS: "); runtime::console.put_hex(red_dump->gs); 

    runtime::console.put_string("\n\n>>>>>>>>>> !SYSTEM HALTED! <<<<<<<<<<\n");

    while (true)
        __asm__ volatile("cli \n hlt");
}
