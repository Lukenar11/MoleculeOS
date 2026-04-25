#pragma once

#include "Heap/Heap.hpp"
#include <Runtime/C/stdint.h>

void* operator new(uintptr_t size) { return kernel::heap::heap.allocate(size); }
void* operator new[](uintptr_t size) { return kernel::heap::heap.allocate(size); }
