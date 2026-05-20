/*
    LICENSE:
        Copyright (c) 2026 Lukenar11 (Luke Matthes)
        MIT licensed
        https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

    DESCRIPTION:
        This class defines the kernel's bump allocator.

        The allocator uses linker-defined symbols ('heap_start' and 'heap_end')
        to determine the valid heap region and moves a single pointer on each allocation, 
        by the amount x.

    NOTES:
        Since this is a simple bump allocator, freeing a memory region is not possible.
*/

#pragma once

#include "utils/heap_pos_marker.h"
#include "system/panic.hpp"
#include <stdint.h>

namespace kernel::heap 
{
    class Bump final {
    private:
        static constexpr uintptr_t ALIGNMENT = 8;
    
        static inline const uintptr_t start = reinterpret_cast<uintptr_t>(&heap_start);
        static inline const uintptr_t end = reinterpret_cast<uintptr_t>(&heap_end);
        uintptr_t current;
    
    public:
        [[nodiscard]]
        void* allocate(uintptr_t allocated_bytes);

        void rewind(const uintptr_t marker) noexcept;

        [[nodiscard]] inline uintptr_t mark() const noexcept { return current; }
        [[nodiscard]] inline uintptr_t used() const noexcept { return current - start; }
        [[nodiscard]] inline uintptr_t remaining() const noexcept { return end - current; }

        inline Bump() noexcept { current = start; }
        ~Bump() noexcept = default;
    };

    // GLOBAL Heap object
    extern Bump bump;
} // namespace kernel::heap
