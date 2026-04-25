#pragma once

#include <Runtime/C/stdint.h>

/*
    "__attribute__((packed))" prevents the compiler from padding the structure 
    with filler bytes.  
    The entries of the "IDTDescriptorPTR" structure must match the layout exactly.  
    Any deviation (e.g., due to alignment) would result in the wrong address being loaded. 
    Therefore, "packed" is absolutely necessary here.
*/

namespace kernel::idt 
{
    struct IDTDescriptorPTR final {
        uint16_t limit;
        uint32_t base;
    } __attribute__((packed));
} // namespace kernel::idt 
