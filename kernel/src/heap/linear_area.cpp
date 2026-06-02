/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This class defines the entire system heap using a linear-area allocator.

    The allocator uses linker-defined symbols ("heap_start" and "heap_end")
    to determine the valid heap area and moves a single pointer with each allocation
    by the amount x.

NOTES:
    Always use "mark()" before allocating memory 
    so that you can free the memory again using (rewind).
*/

#include "heap/linear_area.hpp"

namespace kernel::heap 
{
    [[nodiscard]]
    void* Linear_Area::allocate(uintptr_t allocated_bytes) {
        const uint32_t one = 1;
        const uintptr_t alignment = 8;

        if (allocated_bytes == 0) [[unlikely]]
            return nullptr;
    
        allocated_bytes = (allocated_bytes + (alignment - one)) & ~(alignment - one);
        uintptr_t aligned = (current + (alignment - one)) & ~(alignment - one);
    
        if (aligned + allocated_bytes > end) [[unlikely]]
            system::panic("Out of heap memory", "optimize your memory usage");
    
        void* result = reinterpret_cast<void*>(aligned);
        current = aligned + allocated_bytes;
        return result;
    }

    void Linear_Area::rewind(const uintptr_t marker) noexcept {
        static const char* invalid_heap_rewind_panic_message = "Invalid heap rewind";

        if ((marker < start) || (marker > current) || (marker > end)) [[unlikely]]
            system::panic(
                invalid_heap_rewind_panic_message,
                "Marker is outside the valid heap range or not aligned."
            );

        const uintptr_t alignment = 8;
        if ((marker % alignment) != 0) [[unlikely]]
            system::panic(
                invalid_heap_rewind_panic_message,
                "Marker is not aligned to allocator alignment."
            );

        current = marker;
    }
    
    // GLOBAL Heap object
    Linear_Area linear_area;
} // namespace kernel::heap
