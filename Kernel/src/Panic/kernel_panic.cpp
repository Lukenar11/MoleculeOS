#include "Panic/kernel_panic.hpp"

extern "C" [[noreturn]] 
void kernel_panic(const char* error_message) 
{
    runtime::console.reset();
    runtime::console.set_char_colors(
        drivers::vga::VGAColors::RED,
        drivers::vga::VGAColors::BLACK
    );
    
    runtime::console.put_string(">>>>>>>>>>  KERNEL PANIC  <<<<<<<<<<\n\n");
    runtime::console.put_string(error_message);
    runtime::console.put_string("\n\n>>>>>>>>>> SYSTEM HALTED. <<<<<<<<<<");
    
    while (true)
        asm volatile(
            "cli\n"
            "hlt\n"
        );
}
