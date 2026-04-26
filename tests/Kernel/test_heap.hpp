#pragma once

#include <Runtime/CPP/ConsoleIO.hpp>
#include <Runtime/CPP/memory.hpp>

void test_bump_raw() 
{
    runtime::console.reset();

    runtime::console.printf("heap_start: %p\n", &heap_start);
    for (uint32_t size = 1; size < 23; size++) {
        void* ptr = kernel::heap::heap.allocate(size);
        uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
        
        bool is_aligned = (addr % 8) == 0;
        runtime::console.printf(
            "Size: %d -> Ptr: %p -> Aligned: %s (addr %% 8 = %d) | Used: %d\n", 
            size, 
            ptr, 
            is_aligned ? "YES" : "NO", 
            addr % 8, 
            kernel::heap::heap.used()
        );
    }

    runtime::console.printf("heap_end: %p\n", &heap_end);
    runtime::console.printf("Remaining: %p bytes", kernel::heap::heap.remaining());
}

void test_new_raw()
{
    runtime::console.reset();

    runtime::console.printf("heap_start: %p\n", &heap_start);
    for (uint32_t size = 1; size < 23; size++) {
        uint8_t* ptr = new uint8_t[size];
        uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);

        bool is_aligned = (addr % 8) == 0;
        runtime::console.printf(
            "new[%d] -> Ptr: %p -> Aligned: %s (addr %% 8 = %d) | Used: %d\n",
            size,
            ptr,
            is_aligned ? "YES" : "NO",
            addr % 8,
            kernel::heap::heap.used()
        );
    }

    runtime::console.printf("heap_end: %p\n", &heap_end);
    runtime::console.printf("Remaining: %u bytes", kernel::heap::heap.remaining());
}
