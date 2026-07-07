/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file overloads the 'new' and 'delete' operators to allocate and 
    deallocate memory on the heap without requiring extensive reading and 
    in compliance with the C++ standard.

    The overloaded 'new' and 'delete' operators are essentially wrappers around 
    the heap class methodes '.allocate()' and '.deallocate()'.

NOTES:
    Since the C++ standard requires this, 'new' and 'delete' must be used in the 
    global namespace and not in the 'kernel::heap' namespace.
*/

#include "heap/new_delete.hpp"

void* operator new(const size_t allocated_bytes)
{
    return kernel::heap::Block_Allocator::allocate(allocated_bytes);
}

void* operator new[](const size_t allocated_bytes)
{
    return kernel::heap::Block_Allocator::allocate(allocated_bytes);
}

void operator delete(void* ptr) noexcept
{
    kernel::heap::Block_Allocator::deallocate(ptr);
}

void operator delete[](void* ptr) noexcept
{
    kernel::heap::Block_Allocator::deallocate(ptr);
}
