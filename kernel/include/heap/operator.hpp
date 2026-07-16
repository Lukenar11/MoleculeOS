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
    the heap class methods '::allocate()' and '::deallocate()'.

NOTES:
    Since the C++ standard requires this, 'new' and 'delete' must be used in the 
    global namespace and not in the 'kernel::heap' namespace.
*/

#pragma once

#include "heap/block.hpp"
#include <stdint.h>

void* operator new(const size_t allocated_bytes);
void* operator new[](const size_t allocated_bytes);

void operator delete(void* ptr) noexcept;
void operator delete[](void* ptr) noexcept;
