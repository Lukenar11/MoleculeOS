#pragma once

#include <kernel_api.hpp>
#include <text_output.hpp>

void test_allocator_raw()
{
    runtime::text_output.reset();

    runtime::text_output.put_string("heap_start: ");
    runtime::text_output.put_uint(reinterpret_cast<uint32_t>(&kernel::heap::heap_start));

    runtime::text_output.put_char('\n');
    for (uint32_t n = 1; n < 4; n++) {
        void* ptr = kernel::heap::block.allocate(n);
        uint32_t addr = reinterpret_cast<uint32_t>(ptr);

        runtime::text_output.put_string(((addr % 8) == 0) ? "\naligend" : "\nno aligend");

        runtime::text_output.put_string("\nsize: ");
        runtime::text_output.put_uint(n);

        runtime::text_output.put_string("\nptr: ");
        runtime::text_output.put_uint(uint32_t(ptr));

        runtime::text_output.put_string("\naddress: ");
        runtime::text_output.put_uint(addr);

        runtime::text_output.put_char('\n');
    }

    runtime::text_output.put_string("\nheap_end: ");
    runtime::text_output.put_uint(reinterpret_cast<uint32_t>(&kernel::heap::heap_end));
}

void test_allocator()
{
    runtime::text_output.reset();

    runtime::text_output.put_string("heap_start: ");
    runtime::text_output.put_uint(reinterpret_cast<uint32_t>(&kernel::heap::heap_start));

    runtime::text_output.put_char('\n');
    for (uint32_t n = 1; n < 4; n++) {
        void* ptr = kernel::heap::block.allocate(n);
        uint32_t addr = reinterpret_cast<uint32_t>(ptr);

        runtime::text_output.put_string(((addr % 8) == 0) ? "\naligend" : "\nno aligend");

        runtime::text_output.put_string("\nsize: ");
        runtime::text_output.put_uint(n);

        runtime::text_output.put_string("\nptr: ");
        runtime::text_output.put_ptr(uint32_t(ptr));

        runtime::text_output.put_string("\naddress: ");
        runtime::text_output.put_uint(addr);

        kernel::heap::block.deallocate(ptr);

        runtime::text_output.put_char('\n');
    }

    runtime::text_output.put_string("\nheap_end: ");
    runtime::text_output.put_uint(reinterpret_cast<uint32_t>(&kernel::heap::heap_end));
}
