#pragma once

#include <Runtime/CPP/ConsoleIO.hpp>
#include <Runtime/CPP/memory.hpp>

void test_bump_raw() {

    runtime::console.put_string("=== BUMP RAW TEST ===\n");

    void* a = kernel::heap::heap.allocate(9);
    runtime::console.printf("A = %p\n", a);

    runtime::console.printf("Used: %p bytes\n", kernel::heap::heap.used());
    runtime::console.printf("Remaining: %p bytes\n", kernel::heap::heap.remaining());
}

void test_bump_new_delete() {

    runtime::console.put_string("=== BUMP NEW/DELETE TEST ===\n");

    uint32_t* x = new uint32_t(123);
    runtime::console.printf("x = %p, *x = %d\n", x, *x);

    uint32_t* arr = new uint32_t[10];
    for (uint32_t i = 0; i < 10; i++)
        arr[i] = i * 2;

    runtime::console.printf("arr = %p, arr[5] = %d\n", arr, arr[5]);
}
