#pragma once

#include "utils/heap_pos_marker.h"
#include "Panic/kernel_panic.hpp"
#include <Runtime/C/stdint.h>

namespace kernel::heap 
{
    class Heap {
        private:
            uintptr_t current;

            static constexpr uintptr_t ALIGNMENT = 8;
    
        public:
            void* allocate(uintptr_t size);

            [[nodiscard]]
            inline uintptr_t used() const noexcept { 
                return current - reinterpret_cast<uintptr_t>(&heap_start); 
            }

            [[nodiscard]]
            inline uintptr_t remaining() const noexcept { 
                return reinterpret_cast<uintptr_t>(&heap_end) - current; 
            }

            Heap() noexcept;
            ~Heap() noexcept = default;
    };

    extern Heap heap;
} // namespace kernel::heap
