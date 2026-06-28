/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This class defines the entire system heap using a stack allocator.
    The allocator uses linker-defined symbols 
    ("heap_start" and "heap_end") to determine the valid heap area and 
    moves a single pointer with each allocation by the amount x.

NOTES:
    Always use "mark()" before allocating memory 
    so that you can free the memory again using (rewind).

    Some methods are placed in the header 
    because they are so small that the compiler can inline them.
*/

#pragma once

#include "utils/heap_pos_marker.hpp"
#include "system/panic.hpp"
#include <stdint.h>
#include <kernel_arch_api.hpp>

namespace kernel::heap 
{
    class Stack_Allocator final {
    private:
        static constexpr uint32_t ALIGNMENT = 8;
        static constexpr uint32_t ALIGNMENT_MINUS_ONE = ALIGNMENT - 1;

        uintptr_t start;
        uintptr_t end;
        
        uintptr_t current;
    
    public:
        [[nodiscard]]
        void* allocate(uintptr_t allocated_bytes);

        void rewind(const uintptr_t marker) noexcept;

        [[nodiscard]] 
        inline constexpr uintptr_t mark() const noexcept { 
            return current; 
        }

        [[nodiscard]] 
        inline constexpr uintptr_t used() const noexcept { 
            return current - start;
        }

        [[nodiscard]] 
        inline constexpr uintptr_t remaining() const noexcept { 
            return end - current; 
        }

        inline Stack_Allocator(const uint8_t memory_pool_begin, 
                               const uint8_t memory_pool_end) noexcept {
            start = reinterpret_cast<uintptr_t>(&memory_pool_begin);
            end = reinterpret_cast<uintptr_t>(&memory_pool_end);
            current = start; 
        }
        ~Stack_Allocator() noexcept = default;
    };

    extern Stack_Allocator stack;
} // namespace kernel::heap
