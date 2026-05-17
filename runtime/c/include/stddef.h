/*
    LICENSE:
        Copyright (c) 2026 Lukenar11 (Luke Matthes)
        MIT Licensed
        https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

    DESCRIPTION:
        This defines the basic type "size_t", 
        which is only used by the operator overloads "new" and "new[]".

    NOTES:
*/

#ifndef STDDEF_H
#define STDDEF_H

#define NULL 0

#include <stdint.h>

typedef uint32_t size_t;

#endif // STDDEF_H
