/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT License
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This table is used by the "isr_common_handler" to display 
    meaningful diagnostic messages during exception reporting.

NOTES:
    I had to implement the table in C instead of C++ because 
    my own "array" implementation doesn't allow "char*" and 
    I don't have a data type for strings.
*/

#include "isr/utils/exception_names.h"
