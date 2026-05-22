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
    void* LinearArea::allocate(uintptr_t allocated_bytes) {
        if (allocated_bytes == 0)
            return nullptr;
    
        const uint32_t one = 1;
        allocated_bytes = (allocated_bytes + (ALIGNMENT - one)) & ~(ALIGNMENT - one);
        uintptr_t aligned = (current + (ALIGNMENT - one)) & ~(ALIGNMENT - one);
    
        if (aligned + allocated_bytes > end)
            system::panic("Out of heap memory", "optimize your memory usage");
    
        void* result = reinterpret_cast<void*>(aligned);
        current = aligned + allocated_bytes;
        return result;
    }

    void LinearArea::rewind(const uintptr_t marker) noexcept {
        static const char* rewind_panic_messages[3] = {
            "Invalid heap rewind",
            "Marker is outside the valid heap range or not aligned.",
            "Marker is not aligned to allocator alignment."
        };

        if ((marker < start) || (marker > current) || (marker > end))
            system::panic(rewind_panic_messages[0], rewind_panic_messages[1]);

        if (marker % ALIGNMENT != 0)
            system::panic(rewind_panic_messages[0], rewind_panic_messages[2]);

        current = marker;
    }
    
    // GLOBAL Heap object
    LinearArea linear_area;
} // namespace kernel::heap
