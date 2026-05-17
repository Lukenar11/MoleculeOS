/*
    LICENSE:
        Copyright (c) 2026 Lukenar11 (Luke Matthes)
        MIT licensed
        https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

    DESCRIPTION:
        This class implements the kernel's bump allocator.

        The allocator uses linker-defined symbols ('heap_start' and 'heap_end')
        to determine the valid heap region and moves a single pointer on each allocation, 
        by the amount x.

    NOTES:
        Since this is a simple bump allocator, freeing a memory region is not possible.
*/

#include "heap/heap.hpp"

namespace kernel::heap 
{
    [[nodiscard]]
    void* Heap::allocate(uintptr_t size) {
        if (size == 0)
            return nullptr;
    
        const uint32_t one = 1;
        size = (size + (ALIGNMENT - one)) & ~(ALIGNMENT - one);
        uintptr_t aligned = (current + (ALIGNMENT - one)) & ~(ALIGNMENT - one);
    
        if (aligned + size > reinterpret_cast<uintptr_t>(&heap_end)) {
            system::panic(
                "Out of heap memory", 
                "Consider increasing the heap size or optimizing memory usage."
            );
        }
    
        void* result = reinterpret_cast<void*>(aligned);
        current = aligned + size;
        return result;
    }
    
    // GLOBAL Heap object
    Heap heap;
} // namespace kernel::heap
