/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    C/C++ interface for the common IRQ handler, which is called by "isr_common_stub".

    This function receives a complete "RegisterDump" structure and
    performs uniform exception processing, including diagnostic output
    and system halt.

NOTES:
    Since this function is called by an assembly routine, 
    it is declared as "extern "C"" to ensure compatibility.

    The function "print_reg_dump" is deliberately only in the header 
    because it is quite small, so that the compiler can inline it, 
    since the compiler can better recognize and optimize headers.
*/

#include "isr/isr_common_handler.hpp"

extern "C" 
void isr_common_handler(Register_Dump* reg_dump) 
{
    runtime::text_output.reset();
    runtime::text_output.set_text_color(
        drivers::vga::Text_Mode_Colors::RED,
        drivers::vga::Text_Mode_Colors::BLACK
    );
    
    const uint32_t n = reg_dump->interrupt_number;
    const uint32_t all_available_expectations = 0x6A;
    const char* exception = (n < all_available_expectations)
                            ? exception_names[n]
                            : "Unknown Exception";

    runtime::text_output.put_string(">>>>>>>>>> !KERNEL PANIC! <<<<<<<<<<");
    print_reg_dump("\n\nError Code: ", reg_dump->error_code);

    runtime::text_output.put_string("\nException: ");
    runtime::text_output.put_string(exception);
    runtime::text_output.put_char(' ');
    runtime::text_output.put_char('(');
    runtime::text_output.put_hex(reg_dump->interrupt_number);
    runtime::text_output.put_char(')');

    runtime::text_output.put_string("\n\nCPU State:");
    print_reg_dump("\n  EIP: ", reg_dump->eip);
    print_reg_dump("\n  CS: ", reg_dump->cs);
    runtime::text_output.put_string("\n  EFLAGS: ");
    runtime::text_output.put_bin(reg_dump->eflags);

    runtime::text_output.put_string("\n\nGeneral Registers:");
    print_reg_dump("\n  EAX: ", reg_dump->eax);
    print_reg_dump("\n  EBX: ", reg_dump->ebx);
    print_reg_dump("\n  ECX: ", reg_dump->ecx);
    print_reg_dump("\n  EDX: ", reg_dump->edx);
    print_reg_dump("\n  ESI: ", reg_dump->esi);
    print_reg_dump("\n  EDI: ", reg_dump->edi);
    print_reg_dump("\n  EBP: ", reg_dump->ebp);

    runtime::text_output.put_string("\n\nSegment Registers:");
    print_reg_dump("\n  DS: ", reg_dump->ds);
    print_reg_dump("\n  ES: ", reg_dump->es);
    print_reg_dump("\n  FS: ", reg_dump->fs);
    print_reg_dump("\n  GS: ", reg_dump->gs);

    kernel::system::hang();
}
