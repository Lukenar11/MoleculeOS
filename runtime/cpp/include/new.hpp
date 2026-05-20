/*
    LICENSE:
        Copyright (c) 2026 Lukenar11 (Luke Matthes)
        MIT Licensed
        https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

    DESCRIPTION:
        This defines the memory allocation operators used in C++ by the system.

    NOTES:
        The operators are intentionally minimal and do not provide proper 
        "delete"/"delete[]" implementations, 
        as the underlying heap is only a simple bump allocator.
*/

#pragma once

#include "Heap/bump.hpp"
#include <stddef.h>

void* operator new(size_t size)
{
    void* ptr = kernel::heap::bump.allocate(size);
    if (!ptr)
        kernel::system::panic("Out of memory", "Heap exhausted in operator \'new\'");

    return ptr;
}

void* operator new[](size_t size)
{
    void* ptr = kernel::heap::bump.allocate(size);
    if (!ptr)
        kernel::system::panic("Out of memory", "Heap exhausted in operator \'new[]\'");

    return ptr;
}

void operator delete(void* _) { /* ... */ }
void operator delete[](void* _) { /* ... */ }
