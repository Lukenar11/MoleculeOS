#pragma once

#include "kernel/include/heap/linear_area.hpp"
#include <text_output.hpp>

void test_bump_raw()
{
    runtime::text_output.reset();

    runtime::text_output.put_string("heap_start: ");
    runtime::text_output.put_uint(reinterpret_cast<uint32_t>(&heap_start));

    runtime::text_output.put_char('\n');
    for (uint32_t n = 1; n < 4; n++) {
        void* ptr = kernel::heap::linear_area.allocate(n);
        uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
        
        bool is_aligned = (addr % 8) == 0;
        runtime::text_output.put_string((is_aligned) ? "\naligend" : "\nno aligend");

        runtime::text_output.put_string("\nsize: ");
        runtime::text_output.put_uint(n);

        runtime::text_output.put_string("\nptr: ");
        runtime::text_output.put_uint(uint32_t(ptr));

        runtime::text_output.put_string("\naddress: ");
        runtime::text_output.put_uint(addr);

        runtime::text_output.put_string("\nused: ");
        runtime::text_output.put_uint(kernel::heap::linear_area.used());

        runtime::text_output.put_char('\n');
    }

    runtime::text_output.put_string("\nremaining: ");
    runtime::text_output.put_uint(kernel::heap::linear_area.remaining());

    runtime::text_output.put_string("\nheap_end: ");
    runtime::text_output.put_uint(reinterpret_cast<uint32_t>(&heap_end));
}

void test_bump_mark()
{
    runtime::text_output.reset();

    runtime::text_output.put_string("heap_start: ");
    runtime::text_output.put_uint(reinterpret_cast<uint32_t>(&heap_start));

    runtime::text_output.put_char('\n');
    const uintptr_t marker = kernel::heap::linear_area.mark();
    for (uint32_t n = 1; n < 4; n++) {
        void* ptr = kernel::heap::linear_area.allocate(n);
        uintptr_t addr = reinterpret_cast<uintptr_t>(ptr);
        
        bool is_aligned = (addr % 8) == 0;
        runtime::text_output.put_string((is_aligned) ? "\naligend" : "\nno aligend");

        runtime::text_output.put_string("\nsize: ");
        runtime::text_output.put_uint(n);

        runtime::text_output.put_string("\nptr: ");
        runtime::text_output.put_uint(uint32_t(ptr));

        runtime::text_output.put_string("\naddress: ");
        runtime::text_output.put_uint(addr);

        runtime::text_output.put_string("\nused: ");
        runtime::text_output.put_uint(kernel::heap::linear_area.used());

        runtime::text_output.put_char('\n');
    }

    kernel::heap::linear_area.rewind(marker);

    runtime::text_output.put_string("\nused: ");
    runtime::text_output.put_uint(kernel::heap::linear_area.used());

    runtime::text_output.put_string("\nremaining: ");
    runtime::text_output.put_uint(kernel::heap::linear_area.remaining());

    runtime::text_output.put_string("\nheap_end: ");
    runtime::text_output.put_uint(reinterpret_cast<uint32_t>(&heap_end));
}
