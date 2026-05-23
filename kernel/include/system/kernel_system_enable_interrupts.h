/*
    LICENSE:
        Copyright (c) 2026 Lukenar11 (Luke Matthes)
        MIT Licensed
        https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

    DESCRIPTION:
        This file exposes the low-level CPU interrupt enebling routine.
        The routine uses the i386 instruction "sti" to disable interrupts.

    NOTES:
        The internal implementation is located in "kernel_system_enable_interrupts.asm".

        This header is written in C rather than C++ to ensure maximum compatibility,
        as Assembly provides a C interface, but not a C++ interface.
*/

#ifndef KERNEL_SYSTEM_ENABLE_INTERRUPTS_H
#define KERNEL_SYSTEM_ENABLE_INTERRUPTS_H

#ifdef __cplusplus
extern "C" {
#endif

void kernel_system_enable_interrupts(void);

#ifdef __cplusplus
}
#endif

#endif // KERNEL_SYSTEM_ENABLE_INTERRUPTS_H
