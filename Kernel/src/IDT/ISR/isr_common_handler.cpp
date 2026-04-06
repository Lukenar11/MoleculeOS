#include "IDT/ISR/isr_common_handler.hpp"

extern "C" void isr_common_handler(RegisterDump* register_dump) {

    runtime::console.reset();
    runtime::console.set_char_colors(
        drivers::vga::VGAColors::RED,
        drivers::vga::VGAColors::BLACK
    );

    runtime::console.put_string(">>>>>>>>>> !KERNEL PANIC! <<<<<<<<<<");

    runtime::console.put_string("\n\nException: ");
    runtime::console.put_string(exception_names[register_dump->interrupt_number]);
    runtime::console.put_string(" (");
    runtime::console.put_hex(register_dump->interrupt_number);
    runtime::console.put_string(")");

    runtime::console.put_string("\n\nCPU State:");
    print_register_dump("\n  EIP: ",    register_dump->eip);
    runtime::console.put_string("\n  EFLAGS: ");
    runtime::console.put_bin(register_dump->eflags);

    runtime::console.put_string("\n\nGeneral Registers:");
    print_register_dump("\n  EAX: ", register_dump->eax);
    print_register_dump("\n  EBX: ", register_dump->ebx);
    print_register_dump("\n  ECX: ", register_dump->ecx);
    print_register_dump("\n  EDX: ", register_dump->edx);
    print_register_dump("\n  ESI: ", register_dump->esi);
    print_register_dump("\n  EDI: ", register_dump->edi);
    print_register_dump("\n  EBP: ", register_dump->ebp);

    runtime::console.put_string("\n\nSegment Registers:");
    print_register_dump("\n  DS: ", register_dump->ds);
    print_register_dump("\n  ES: ", register_dump->es);
    print_register_dump("\n  FS: ", register_dump->fs);
    print_register_dump("\n  GS: ", register_dump->gs);

    runtime::console.put_string("\n\n>>>>>>>>>> !SYSTEM HALTED! <<<<<<<<<<");

    while (true)
        __asm__ volatile("cli \n hlt");
}
