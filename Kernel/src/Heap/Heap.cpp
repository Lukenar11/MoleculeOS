#include "Heap/Heap.hpp"

namespace kernel::heap 
{
    Heap::Heap() noexcept { 
        current = reinterpret_cast<uintptr_t>(&heap_start); 
    }

    void* Heap::allocate(uintptr_t size) {
        if (size == 0)
            return nullptr;
    
        size = (size + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1);
        uintptr_t aligned = (current + (ALIGNMENT - 1)) & ~(ALIGNMENT - 1);
    
        if (aligned + size > reinterpret_cast<uintptr_t>(&heap_end)) {
            kernel_panic("Out of heap memory");
        }
    
        void* result = reinterpret_cast<void*>(aligned);
        current = aligned + size;
        return result;
    }
    
    Heap heap;
} // namespace kernel::heap
