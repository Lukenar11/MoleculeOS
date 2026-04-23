#pragma once

#include "utils/HeapBlock.hpp"
#include "utils/heap_pos_marker.h"
#include <Runtime/C/stdint.h>
#include <Runtime/CPP/Array.hpp>

namespace kernel::heap {

    class Heap {

        private:
            HeapBlock* first_block;
    
        public:
            void* allocate(const uint32_t size) noexcept;
            void deallocate(void* heap_block_ptr) noexcept;

            Heap() noexcept;
            ~Heap() noexcept = default;
    };

    extern Heap heap;
} // namespace kernel::heap
