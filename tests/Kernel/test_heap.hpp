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

void test_new()
{
    runtime::text_output.reset();

    runtime::text_output.put_string("heap_start: ");
    runtime::text_output.put_uint(reinterpret_cast<uint32_t>(&kernel::heap::heap_start));
    runtime::text_output.put_char('\n');

    for (uint32_t n = 1; n < 4; n++) {

        uint8_t* ptr = new uint8_t(n);
        uint32_t addr = reinterpret_cast<uint32_t>(ptr);

        runtime::text_output.put_string(((addr % 8) == 0) ? "\naligend" : "\nno aligend");

        runtime::text_output.put_string("\nsize: ");
        runtime::text_output.put_uint(n);

        runtime::text_output.put_string("\nptr: ");
        runtime::text_output.put_ptr(uint32_t(ptr));

        runtime::text_output.put_string("\naddress: ");
        runtime::text_output.put_uint(addr);

        runtime::text_output.put_char('\n');
    }

    runtime::text_output.put_string("\nheap_end: ");
    runtime::text_output.put_uint(reinterpret_cast<uint32_t>(&kernel::heap::heap_end));
}

void test_new_delete()
{
    runtime::text_output.reset();

    runtime::text_output.put_string("heap_start: ");
    runtime::text_output.put_uint(reinterpret_cast<uint32_t>(&kernel::heap::heap_start));
    runtime::text_output.put_char('\n');

    for (uint32_t n = 1; n < 4; n++) {

        uint8_t* ptr = new uint8_t(n);
        uint32_t addr = reinterpret_cast<uint32_t>(ptr);

        runtime::text_output.put_string(((addr % 8) == 0) ? "\naligend" : "\nno aligend");

        runtime::text_output.put_string("\nsize: ");
        runtime::text_output.put_uint(n);

        runtime::text_output.put_string("\nptr: ");
        runtime::text_output.put_ptr(uint32_t(ptr));

        runtime::text_output.put_string("\naddress: ");
        runtime::text_output.put_uint(addr);

        delete ptr;

        runtime::text_output.put_char('\n');
    }

    runtime::text_output.put_string("\nheap_end: ");
    runtime::text_output.put_uint(reinterpret_cast<uint32_t>(&kernel::heap::heap_end));
}

void test_new_delete_array()
{
    runtime::text_output.reset();

    runtime::text_output.put_string("heap_start: ");
    runtime::text_output.put_uint(reinterpret_cast<uint32_t>(&kernel::heap::heap_start));
    runtime::text_output.put_char('\n');

    for (uint32_t n = 1; n < 4; n++) {

        uint32_t count = n * 5;
        uint32_t bytes = count * sizeof(uint32_t);

        uint32_t* arr = new uint32_t[count];
        uint32_t addr = reinterpret_cast<uint32_t>(arr);

        runtime::text_output.put_string(((addr % 8) == 0) ? "\naligend" : "\nno aligend");

        runtime::text_output.put_string("\narray elements: ");
        runtime::text_output.put_uint(count);

        runtime::text_output.put_string("\nbytes: ");
        runtime::text_output.put_uint(bytes);

        runtime::text_output.put_string("\nptr: ");
        runtime::text_output.put_ptr(uint32_t(arr));

        runtime::text_output.put_string("\naddress: ");
        runtime::text_output.put_uint(addr);

        delete[] arr;

        runtime::text_output.put_char('\n');
    }

    runtime::text_output.put_string("\nheap_end: ");
    runtime::text_output.put_uint(reinterpret_cast<uint32_t>(&kernel::heap::heap_end));
}

void test_new_array()
{
    runtime::text_output.reset();

    runtime::text_output.put_string("heap_start: ");
    runtime::text_output.put_uint(reinterpret_cast<uint32_t>(&kernel::heap::heap_start));
    runtime::text_output.put_char('\n');

    for (uint32_t n = 1; n < 4; n++) {

        uint32_t count = n * 5;
        uint32_t bytes = count * sizeof(uint32_t);

        uint32_t* arr = new uint32_t[count];
        uint32_t addr = reinterpret_cast<uint32_t>(arr);

        runtime::text_output.put_string(((addr % 8) == 0) ? "\naligend" : "\nno aligend");

        runtime::text_output.put_string("\narray elements: ");
        runtime::text_output.put_uint(count);

        runtime::text_output.put_string("\nbytes: ");
        runtime::text_output.put_uint(bytes);

        runtime::text_output.put_string("\nptr: ");
        runtime::text_output.put_ptr(uint32_t(arr));

        runtime::text_output.put_string("\naddress: ");
        runtime::text_output.put_uint(addr);

        runtime::text_output.put_char('\n');
    }

    runtime::text_output.put_string("\nheap_end: ");
    runtime::text_output.put_uint(reinterpret_cast<uint32_t>(&kernel::heap::heap_end));
}
