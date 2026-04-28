#include "IDT/ISR/isr_common_handler.hpp"

extern "C" 
void isr_common_handler(RegisterDump* reg_dump) 
{
    runtime::console.reset();
    runtime::console.set_char_colors(
        drivers::vga::VGAColors::RED,
        drivers::vga::VGAColors::BLACK
    );

    runtime::console.put_string(">>>>>>>>>> !KERNEL PANIC! <<<<<<<<<<");

    print_reg_dump("\n\nError Code: ", reg_dump->error_code);
    runtime::console.put_string("\nException: ");
    runtime::console.put_string(exception_names[reg_dump->interrupt_number]);
    runtime::console.put_char(' ');
    runtime::console.put_char('(');
    runtime::console.put_hex(reg_dump->interrupt_number);
    runtime::console.put_char(')');

    runtime::console.put_string("\n\nCPU State:");
    print_reg_dump("\n  EIP: ", reg_dump->eip);
    print_reg_dump("\n  CS: ", reg_dump->cs);
    runtime::console.put_string("\n  EFLAGS: ");
    runtime::console.put_bin(reg_dump->eflags);

    runtime::console.put_string("\n\nGeneral Registers:");
    print_reg_dump("\n  EAX: ", reg_dump->eax);
    print_reg_dump("\n  EBX: ", reg_dump->ebx);
    print_reg_dump("\n  ECX: ", reg_dump->ecx);
    print_reg_dump("\n  EDX: ", reg_dump->edx);
    print_reg_dump("\n  ESI: ", reg_dump->esi);
    print_reg_dump("\n  EDI: ", reg_dump->edi);
    print_reg_dump("\n  EBP: ", reg_dump->ebp);

    runtime::console.put_string("\n\nSegment Registers:");
    print_reg_dump("\n  DS: ", reg_dump->ds);
    print_reg_dump("\n  ES: ", reg_dump->es);
    print_reg_dump("\n  FS: ", reg_dump->fs);
    print_reg_dump("\n  GS: ", reg_dump->gs);

    HaltSystem();
}
