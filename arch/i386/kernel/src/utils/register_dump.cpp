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
    This structure is marked as packed ("__attribute__((packed));") 
    to prevent the compiler from inserting padding bytes. 
    
    The layout of this structure must match the stack layout created by
    the assembly routines, including segment registers, general-purpose
    registers, and exception metadata.
*/

#include "utils/register_dump.hpp"

static_assert(sizeof(Register_Dump) == 17 * 4, "RegisterDump size mismatch!");
