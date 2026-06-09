#pragma once

#include "kernel/include/filesystem/mofs.hpp"
#include <stdint.h>
#include <text_output.hpp>

void test_create_file()
{
    runtime::text_output.reset();
    runtime::text_output.put_string("Starte create_file-Test...\n");

    void* a = kernel::filesystem::mofs.create_file("hello_world", "txt");
    void* b = kernel::filesystem::mofs.create_file("test", "");

    void* c = kernel::filesystem::mofs.get_inode_by_name_and_format("hello_world", "txt");
    void* d = kernel::filesystem::mofs.get_inode_by_name_and_format("test", "");

    runtime::text_output.put_string("hello_world.txt = ");
    runtime::text_output.put_ptr(reinterpret_cast<uintptr_t>(a));
    runtime::text_output.put_string(" | inode = ");
    runtime::text_output.put_ptr(reinterpret_cast<uintptr_t>(c));

    runtime::text_output.put_char('\n');

    runtime::text_output.put_string("test = ");
    runtime::text_output.put_ptr(reinterpret_cast<uintptr_t>(b));
    runtime::text_output.put_string(" | inode = ");
    runtime::text_output.put_ptr(reinterpret_cast<uintptr_t>(d));
}

void test_read_file()
{
    runtime::text_output.reset();
    runtime::text_output.put_string("Starte read_file-Test...\n");

    kernel::filesystem::Inode* a = kernel::filesystem::mofs.create_file("hello_world", "txt");
    kernel::filesystem::Inode* b = kernel::filesystem::mofs.create_file("test", "");

    uint8_t buffer_a[10] = {};
    if (!kernel::filesystem::mofs.read_file(a, 0, 9, 10, buffer_a)) {
        runtime::text_output.put_string("buffer_a:\n");
        for (uint32_t i = 0; i < 10; i++)
            runtime::text_output.put_int(buffer_a[i]);
    }

    runtime::text_output.put_string("\n\n");

    uint8_t buffer_b[10] = {};
    if (!kernel::filesystem::mofs.read_file(b, 4, 5, 10, buffer_b)) {
        runtime::text_output.put_string("buffer_b:\n");
        for (uint32_t i = 0; i < 10; i++)
            runtime::text_output.put_int(buffer_b[i]);
    }
}
