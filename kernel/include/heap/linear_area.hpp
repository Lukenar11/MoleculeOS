/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
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

#pragma once

#include "utils/heap_pos_marker.h"
#include "system/panic.hpp"
#include <stdint.h>

namespace kernel::heap 
{
    class Linear_Area final {
    private:
        static constexpr uintptr_t ALIGNMENT = 8;
    
        const uintptr_t start = reinterpret_cast<uintptr_t>(&heap_start);
        const uintptr_t end = reinterpret_cast<uintptr_t>(&heap_end);
        uintptr_t current;
    
    public:
        [[nodiscard]]
        void* allocate(uintptr_t allocated_bytes);

        void rewind(const uintptr_t marker) noexcept;

        [[nodiscard]] inline uintptr_t mark() const noexcept { return current; }
        [[nodiscard]] inline uintptr_t used() const noexcept { return current - start; }
        [[nodiscard]] inline uintptr_t remaining() const noexcept { return end - current; }

        inline Linear_Area() noexcept { current = start; }
        ~Linear_Area() noexcept = default;
    };

    // GLOBAL Heap object
    extern Linear_Area linear_area;
} // namespace kernel::heap
