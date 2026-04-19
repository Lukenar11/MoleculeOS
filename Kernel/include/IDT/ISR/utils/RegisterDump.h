#ifndef HELPERS_H
#define HELPERS_H

#include <Runtime/C/stdint.h>

/*
    "__attribute__((packed))" prevents the compiler from padding the structure with filler bytes.  
    The entries of the "RegisterDump" structure must match the layout exactly.  
    Any deviation (e.g., due to alignment) would lead to incorrect register and error code outputs in the "isr_common_handler".  
    Therefore, "packed" is absolutely necessary here.

    Note: 
    I deliberately wrote this code in C instead of C++ to be maximally compatible with the ISR Assembly components, 
    which is why I also use the dumb "typedef struct" system.
*/

#ifdef __cplusplus
    extern "C" {
#endif

    typedef struct RegisterDump RegisterDump;
    struct RegisterDump {

        uint32_t gs, fs, es, ds;

        uint32_t eax;
        uint32_t ecx;
        uint32_t edx;
        uint32_t ebx;
        uint32_t esp_dummy;
        uint32_t ebp;
        uint32_t esi;
        uint32_t edi;

        uint32_t interrupt_number;
        uint32_t error_code;

        uint32_t eip, cs, eflags;
    } __attribute__((packed));
#ifdef __cplusplus
    }
#endif

#endif // HELPERS_H
