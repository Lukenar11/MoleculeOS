/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    C++ interface for the common 'IRQ handler', 
    which is called by 'isr_stub'.

    This function receives a complete 'Registers' structure and
    performs uniform exception processing, including diagnostic output
    and system halt.

NOTES:
    Some functions are placed in the header 
    because they are so small that the compiler can inline them.
*/

#include "isr_handler.hpp"

#include <drivers.hpp>

namespace kernel::idt::isr
{
    extern "C"
    void isr_handler(const Registers* reg_dump) {

        stdlib::console::reset();
        stdlib::console::set_text_color(drivers::vga::Text_Mode_Color::BLACK,
                                        drivers::vga::Text_Mode_Color::RED);

        const uint32_t all_available_expectations = 0x6A;
        const char* exception = (reg_dump->interrupt_number < all_available_expectations)
                                ? exception_names[reg_dump->interrupt_number]
                                : "Unknown Exception";
    
        stdlib::console::put_string(">>>>>>>>>> !KERNEL PANIC! <<<<<<<<<<");
        print_reg_dump("\n\nError Code: ", reg_dump->error_code);
    
        stdlib::console::put_string("\nException:  ");
        stdlib::console::put_string(exception);
        stdlib::console::put_char(' ');
        stdlib::console::put_char('(');
        stdlib::console::put_hex(reg_dump->interrupt_number);
        stdlib::console::put_char(')');
    
        stdlib::console::put_string("\n\nCPU State:");
        print_reg_dump("\n\tEIP:    ", reg_dump->eip);
        print_reg_dump("\n\tCS:     ", reg_dump->cs);
        stdlib::console::put_string("\n\tEFLAGS: ");
        stdlib::console::put_bin(reg_dump->eflags);
    
        stdlib::console::put_string("\n\nGeneral Registers:");               
        print_reg_dump("\n\tEAX: ", reg_dump->eax);
        print_reg_dump("\n\tEBX: ", reg_dump->ebx);
        print_reg_dump("\n\tECX: ", reg_dump->ecx);
        print_reg_dump("\n\tEDX: ", reg_dump->edx);
        print_reg_dump("\n\tESI: ", reg_dump->esi);
        print_reg_dump("\n\tEDI: ", reg_dump->edi);
        print_reg_dump("\n\tEBP: ", reg_dump->ebp);

        stdlib::console::put_string("\n\nSegment Registers:");
        print_reg_dump("\n\tDS: ", reg_dump->ds);
        print_reg_dump("\n\tES: ", reg_dump->es);
        print_reg_dump("\n\tFS: ", reg_dump->fs);
        print_reg_dump("\n\tGS: ", reg_dump->gs);

        sys::hang();
    }
} // namespace kernel::isr
