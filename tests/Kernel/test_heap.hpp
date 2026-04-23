#pragma once

#include <Runtime/CPP/ConsoleIO.hpp>
#include <Runtime/CPP/memory.hpp>

void test_heap_raw() {

    runtime::console.put_string("=== RAW HEAP TEST ===\n");

    void* a = kernel::heap::heap.allocate(32);
    runtime::console.printf("Allocated A @ %p\n", (uintptr_t)a);

    void* b = kernel::heap::heap.allocate(64);
    runtime::console.printf("Allocated B @ %p\n", (uintptr_t)b);

    kernel::heap::heap.deallocate(a);
    runtime::console.printf("Deallocated A\n");

    void* c = kernel::heap::heap.allocate(16);
    runtime::console.printf("Allocated C @ %p\n", (uintptr_t)c);

    kernel::heap::heap.deallocate(b);
    kernel::heap::heap.deallocate(c);

    runtime::console.put_string("Raw heap test done.\n\n");
}

void test_heap_new_delete() {

    runtime::console.put_string("=== NEW/DELETE TEST ===\n");

    int* x = new int(123);
    runtime::console.printf("x = %p, *x = %d\n", (uintptr_t)x, *x);

    int* arr = new int[10];
    for (int i = 0; i < 10; i++)
        arr[i] = i * 2;

    runtime::console.printf("arr = %p, arr[5] = %d\n", (uintptr_t)arr, arr[5]);

    delete x;
    delete[] arr;

    runtime::console.put_string("new/delete test done.\n\n");
}
