/*
    LICENSE:
        Copyright (c) 2026 Lukenar11 (Luke Matthes)
        MIT Licensed
        https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

    DESCRIPTION:
        This header exposes the low-level CPU halt routine.

        The routine disables interrupts and enters an infinite halt loop,
        ensuring that the CPU remains in a safe, non-executing state.

    NOTES:
        The internal implementation is located in "kernel_system_hang.asm".

        This header is written in C rather than C++ to ensure maximum compatibility,
        as Assembly provides a C interface, but not a C++ interface.
*/

#ifndef KERNEL_SYSTEM_HANG_H
#define KERNEL_SYSTEM_HANG_H

#ifdef __cplusplus
extern "C" {
#endif

void kernel_system_hang(void);

#ifdef __cplusplus
}
#endif

#endif // KERNEL_SYSTEM_HANG_H
