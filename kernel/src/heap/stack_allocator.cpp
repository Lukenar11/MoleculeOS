/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This class defines the entire system heap using a stack allocator.

    The allocator uses linker-defined symbols ("heap_start" and "heap_end") 
    to determine the valid heap area and 
    moves a single pointer with each allocation by the amount x.

NOTES:
    Always use "mark()" before allocating memory 
    so that you can free the memory again using (rewind).

    Some methods are placed in the header 
    because they are so small that the compiler can inline them.
*/

#include "heap/stack_allocator.hpp"

namespace kernel::heap 
{
    [[nodiscard]]
    void* Stack_Allocator::allocate(uintptr_t allocated_bytes) {;
        if (allocated_bytes == 0) [[unlikely]]
            return nullptr;
    
        allocated_bytes = (allocated_bytes + ALIGNMENT_MINUS_ONE) & ~ALIGNMENT_MINUS_ONE ;
        uintptr_t aligned = (current + ALIGNMENT_MINUS_ONE) & ~ALIGNMENT_MINUS_ONE;
    
        if (aligned + allocated_bytes > end) [[unlikely]]
            system::panic("Out of heap memory");
    
        void* result = reinterpret_cast<void*>(aligned);
        current = aligned + allocated_bytes;
        return result;
    }

    void Stack_Allocator::rewind(const uintptr_t marker) noexcept {
        if ((marker < start) || (marker > current) || (marker > end)) [[unlikely]]
            system::panic(
                "Marker is outside the valid heap range or not aligned."
            );

        if ((marker % ALIGNMENT) != 0) [[unlikely]]
            system::panic(
                "Marker is not aligned to allocator ALIGNMENT."
            );

        current = marker;
    }
    
    Stack_Allocator stack(heap_start, heap_end);
} // namespace kernel::heap
