/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This library contains a C-like runtime for basic string and
    memory manipulation.

    The actual implementation of the runtime is hidden in "memory_manip" and
    "string_manip"; this is merely a C interface.

NOTES:
    This header is purely an ABI compatibility layer; 
    it is included only because all 
    C and C++ compilers require a basic C runtime and 
    do not accept custom implementations with names or
    signatures different from those described in the C standard.

    The wrappers for these functions are located in the 
    files "compiler_c_api.hpp" and "compiler_c_api.cpp".
*/

#include "utils/string.h"
