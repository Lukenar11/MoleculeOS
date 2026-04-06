#ifndef HELPERS_H
#define HELPERS_H

#include <Runtime/C/stdint.h>

#ifdef __cplusplus
    extern "C" {
#endif

    struct RegisterDump {

        uint32_t ds, es, fs, gs;
        uint32_t edi, esi, ebp, ebx, edx, ecx, eax;
        uint32_t interrupt_number, error_code;
        uint32_t eip, cs, eflags;
    };
#ifdef __cplusplus
    }
#endif

#endif // HELPERS_H
