/*
    LICENSE:
        Copyright (c) 2026 Lukenar11 (Luke Matthes)
        MIT Licensed
        https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
    
    DESCRIPTION:
        This is the "RegisterDump structure", which represents the
        exact CPU state saved by the ISR and IRQ assembly stubs before
        transferring control to the C-level handlers.
    
    NOTES:
        The layout of this structure must match the stack layout created by
        the assembly routines, including segment registers, general-purpose
        registers, and exception metadata.

        This header is written in C rather than C++ to ensure maximum compatibility,
        as Assembly provides a C interface, but not a C++ interface.
*/

#include "utils/register_dump.h"
