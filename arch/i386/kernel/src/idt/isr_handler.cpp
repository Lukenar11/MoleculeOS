/*
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

namespace kernel::idt::isr
{
    extern "C"
    void isr_handler(Registers* reg_dump) {
        const bool clear_screen_if_hight_limit_reached = false;

        runtime::Text_Output::reset();
        runtime::Text_Output::set_text_color(drivers::vga::Text_Mode_Colors::RED,
                                             drivers::vga::Text_Mode_Colors::BLACK);

        const uint32_t all_available_expectations = 0x6A;
        const char* exception = (reg_dump->interrupt_number < all_available_expectations)
                                ? exception_names[reg_dump->interrupt_number]
                                : "Unknown Exception";
    
        runtime::Text_Output::put_string(">>>>>>>>>> !KERNEL PANIC! <<<<<<<<<<", 
                                         clear_screen_if_hight_limit_reached);
        print_reg_dump("\n\nError Code: ", reg_dump->error_code);
    
        runtime::Text_Output::put_string("\nException: ", 
                                         clear_screen_if_hight_limit_reached);
        runtime::Text_Output::put_string(exception);
        runtime::Text_Output::put_char(' ');
        runtime::Text_Output::put_char('(');
        runtime::Text_Output::put_hex(reg_dump->interrupt_number);
        runtime::Text_Output::put_char(')');
    
        runtime::Text_Output::put_string("\n\nCPU State:",
                                         clear_screen_if_hight_limit_reached);
        print_reg_dump("\n\tEIP: ", reg_dump->eip);
        print_reg_dump("\n\tCS: ", reg_dump->cs);
        runtime::Text_Output::put_string("\n\tEFLAGS: ", 
                                         clear_screen_if_hight_limit_reached);
        runtime::Text_Output::put_bin(reg_dump->eflags);
    
        runtime::Text_Output::put_string("\n\nGeneral Registers:", 
                                         clear_screen_if_hight_limit_reached);
        print_reg_dump("\n\tEAX: ", reg_dump->eax);
        print_reg_dump("\n\tEBX: ", reg_dump->ebx);
        print_reg_dump("\n\tECX: ", reg_dump->ecx);
        print_reg_dump("\n\tEDX: ", reg_dump->edx);
        print_reg_dump("\n\tESI: ", reg_dump->esi);
        print_reg_dump("\n\tEDI: ", reg_dump->edi);
        print_reg_dump("\n\tEBP: ", reg_dump->ebp);

        runtime::Text_Output::put_string("\n\nSegment Registers:", 
                                         clear_screen_if_hight_limit_reached);
        print_reg_dump("\n\tDS: ", reg_dump->ds);
        print_reg_dump("\n\tES: ", reg_dump->es);
        print_reg_dump("\n\tFS: ", reg_dump->fs);
        print_reg_dump("\n\tGS: ", reg_dump->gs);

        sys::hang();
    }
} // namespace kernel::isr
