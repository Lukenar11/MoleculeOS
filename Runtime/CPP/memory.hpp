#pragma once

#include "Heap/Heap.hpp"
#include <Runtime/C/stdint.h>

void* operator new(uint32_t size) { return kernel::heap::heap.allocate(size); }
void* operator new[](uint32_t size) { return kernel::heap::heap.allocate(size); }

void operator delete(void* ptr) noexcept { kernel::heap::heap.deallocate(ptr); }
void operator delete[](void* ptr) noexcept { kernel::heap::heap.deallocate(ptr); }

void operator delete(void* ptr, uint32_t) noexcept { kernel::heap::heap.deallocate(ptr); }
void operator delete[](void* ptr, uint32_t) noexcept { kernel::heap::heap.deallocate(ptr); }
