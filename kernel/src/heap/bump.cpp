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

#include "heap/bump.hpp"

namespace kernel::heap 
{
    [[nodiscard]]
    void* Bump::allocate(uintptr_t allocated_bytes) {
        if (allocated_bytes == 0)
            return nullptr;
    
        const uint32_t one = 1;
        allocated_bytes = (allocated_bytes + (ALIGNMENT - one)) & ~(ALIGNMENT - one);
        uintptr_t aligned = (current + (ALIGNMENT - one)) & ~(ALIGNMENT - one);
    
        if (aligned + allocated_bytes > end)
            system::panic(
                "Out of heap memory",
                "optimize your memory usage"
            );
    
        void* result = reinterpret_cast<void*>(aligned);
        current = aligned + allocated_bytes;
        return result;
    }

    void Bump::rewind(const uintptr_t marker) noexcept {
        static const char* rewind_panic_messages[3] = {
            "Invalid heap rewind",
            "Marker is outside the valid heap range or not aligned.",
            "Marker is not aligned to allocator alignment."
        };

        if ((marker < start) || (marker > current) || (marker > end)) {
            system::panic(
                rewind_panic_messages[0],
                rewind_panic_messages[1]
            );
        }

        if (marker % ALIGNMENT != 0) {
            system::panic(
                rewind_panic_messages[0],
                rewind_panic_messages[2]
            );
        }

        current = marker;
    }
    
    // GLOBAL Heap object
    Bump bump;
} // namespace kernel::heap
