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

    void* c = kernel::filesystem::mofs.get_inode_by_filename("hello_world");
    void* d = kernel::filesystem::mofs.get_inode_by_filename("test");

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
