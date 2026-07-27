/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file defines a central runtime API for 
    all architecture-dependent I/O functions.

NOTES:
    Macros that have been defined based on the selected architecture are 
    compiler-specific, e.g. '__i386__' (in GCC/Clang), '_M_IX86' (in MSVC), 
    'i386' (in some cross-compilers), or may not be available at all,
    which is why custom macros are defined for this purpose, 
    e.g. '__I386__' for i386 as shown here.
*/

#pragma once

#ifdef __I386__
#   include "arch/i386/runtime/include/port_io.hpp"
#else
#   error "Architecture is not supported!"
#endif
