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

void test_write_file_read_file_string()
{
    runtime::text_output.reset();
    runtime::text_output.put_string("Starte string read/write Test...\n");

    kernel::filesystem::Inode* x = kernel::filesystem::mofs.create_file("string_test", "txt");

    const char* data = "abcdeABCDE";
    const uint32_t n = 10;

    if (kernel::filesystem::mofs.set_file_content_as_string(x, data, n))
        runtime::text_output.put_string("String write Success\n");
    else
        runtime::text_output.put_string("String write FAIL\n");

    char buffer[11] = {};

    if (kernel::filesystem::mofs.get_file_content_as_string(x, buffer, 11))
        runtime::text_output.put_string("String read Success\n");
    else
        runtime::text_output.put_string("String read FAIL\n");

    for (uint32_t i = 0; i < n; i++) {
        runtime::text_output.put_string("Char ");
        runtime::text_output.put_uint(i);
        runtime::text_output.put_string(": ");
        runtime::text_output.put_char(buffer[i]);
        runtime::text_output.put_char('\n');
    }
}


void test_write_file_read_file_binary()
{
    runtime::text_output.reset();
    runtime::text_output.put_string("Starte binary read/write Test...\n");

    kernel::filesystem::Inode* x = kernel::filesystem::mofs.create_file("bin_test", "bin");

    uint8_t data[10] = { 1, 2, 3, 4, 5, 100, 101, 102, 103, 104 };
    const uint32_t n = 10;

    if (kernel::filesystem::mofs.set_file_content_binary(x, data, n))
        runtime::text_output.put_string("Binary write Success\n");
    else
        runtime::text_output.put_string("Binary write FAIL\n");

    uint8_t buffer[10] = {};

    if (kernel::filesystem::mofs.get_file_content_binary(x, buffer, 10))
        runtime::text_output.put_string("Binary read Success\n");
    else
        runtime::text_output.put_string("Binary read FAIL\n");

    for (uint32_t i = 0; i < n; i++) {
        runtime::text_output.put_string("Byte ");
        runtime::text_output.put_uint(i);
        runtime::text_output.put_string(": ");
        runtime::text_output.put_uint(buffer[i]);
        runtime::text_output.put_char('\n');
    }
}

