/*
    LICENSE:
        Copyright (c) 2026 Lukenar11 (Luke Matthes)
        MIT Licensed
        https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

    DESCRIPTION:
        This header exposes the low-level CPU sleep routine.

        This routine uses the i386 instruction "halt"
        to stop the CPU until an interrupt occurs.

    NOTES:
        The internal implementation is located in "kernel_system_sleep.asm".

        This header is written in C rather than C++ to ensure maximum compatibility,
        as Assembly provides a C interface, but not a C++ interface.
*/

#ifndef KERNEL_SYSTEM_SLEEP_H
#define KERNEL_SYSTEM_SLEEP_H

#ifdef __cplusplus
extern "C" {
#endif

void kernel_system_sleep(void);

#ifdef __cplusplus
}
#endif

#endif // KERNEL_SYSTEM_SLEEP_H
