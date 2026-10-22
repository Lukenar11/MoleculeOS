#pragma once

#include <console.hpp>
#include <types.hpp>
#include <kernel.hpp>

static void fs_print(bool ok, const char* message) noexcept {
    if (ok)
        stdlib::console::put_string("[ OK ] ");
    else
        stdlib::console::put_string("[FAIL] ");

    stdlib::console::put_string(message);
    stdlib::console::put_char('\n');
}

namespace tests
{
    void create_file() noexcept {
        using namespace kernel::filesys;

        stdlib::console::reset();

        File_Header* a = create_file("test", "txt", 32);

        fs_print(a != nullptr, "create_file: file_header created");
        fs_print(a->file_byte_size == 32, "create_file: correct size");
        fs_print(a->used_data_byte_size == 0, "create_file: used size = 0");
    }

    void write_file() noexcept {
        using namespace kernel::filesys;
        
        File_Header* a = find_file("test", "txt");

        uint8_t data1[5] = {1, 2, 3, 4, 5};
        fs_print(write_file(a, 0, 5, 5, data1),
                 "write_file: basic write");

    }

    void read_file() noexcept {
        using namespace kernel::filesys;

        File_Header* a = find_file("test", "txt");

        uint8_t buffer[5];
        fs_print(read_file(a, buffer, 5, 0, 5),
                 "read_file: basic read");

        uint8_t expected[5] = {1, 2, 3, 4, 5};
        for (uint32_t i = 0; i < 5; i++)
            fs_print(buffer[i] == expected[i], "read_file: content matches");
    }

    void append_file() noexcept {
        using namespace kernel::filesys;
        
        File_Header* a = find_file("test", "txt");

        uint8_t data2[3] = {9, 9, 9};
        fs_print(append_file(a, data2, 3),
                 "append_file: basic append");

        fs_print(a->used_data_byte_size == 3,
                 "append_file: used size updated");
    }

    void clear_file() noexcept {
        using namespace kernel::filesys;
        
        File_Header* a = find_file("test", "txt");

        fs_print(clear_file(a),
                 "clear_file: cleared");

        fs_print(a->used_data_byte_size == 0,
                 "clear_file: used size reset");

        uint8_t buffer[32];
        read_file(a, buffer, 32, 0, 32);

        for (uint32_t i = 0; i < 32; i++);
            // fs_print(buffer[i] == 0, "clear_file: all bytes zero");
    }

    void rename_file() noexcept {
        using namespace kernel::filesys;
        
        fs_print(rename_file("test", "txt",
                                                       "log", "bin"),
                 "rename_file: renamed");

        fs_print(find_file("log", "bin") != nullptr,
                 "rename_file: new name exists");

        fs_print(find_file("test", "txt") == nullptr,
                 "rename_file: old name removed");
    }

    void copy_file() noexcept {
        using namespace kernel::filesys;
        
        fs_print(copy_file("log", "bin",
                                                     "backup", "bin"),
                 "copy_file: copied");

        File_Header* src = find_file("log", "bin");
        File_Header* dst = find_file("backup", "bin");

        fs_print(dst != nullptr, "copy_file: backup exists");
        fs_print(dst->file_byte_size == src->file_byte_size,
                 "copy_file: size matches");
        fs_print(dst->used_data_byte_size == src->used_data_byte_size,
                 "copy_file: used size matches");
    }

    void delete_file() noexcept {
        using namespace kernel::filesys;
        
        fs_print(delete_file("log", "bin"),
                 "delete_file: deleted");

        fs_print(find_file("log", "bin") == nullptr,
                 "delete_file: file removed");
    }

    void error_cases() noexcept {
        using namespace kernel::filesys;
        
        File_Header* a = find_file("backup", "bin");

        uint8_t data1[5] = {1, 2, 3, 4, 5};

        fs_print(!write_file(a, 1000, 10, 10, data1),
                 "write_file: out of bounds fails");
                 
        fs_print(!append_file(a, data1, 1000),
                 "append_file: no space fails");

        uint8_t small_buf[2];
        fs_print(!read_file(a, small_buf, 2, 0, 5),
                 "read_file: buffer too small fails");

        create_file("x", "txt", 10);
        fs_print(!copy_file("x", "txt", "x", "txt"),
                 "copy_file: dest exists fails");
    }

    void resize_file_size() noexcept {
        using namespace kernel::filesys;

        stdlib::console::reset();

        File_Header* file_header = create_file("resize", "bin", 16);
        fs_print(file_header != nullptr, 
                 "resize_file_size: create test file");

        uint8_t initial[16];
        for (uint32_t i = 0; i < 16; i++)
            initial[i] = i;

        write_file(file_header, 0, 16, 16, initial);

        fs_print(resize_file_size(file_header, 32),
                 "resize_file_size: grow file to 32 bytes");

        fs_print(file_header->file_byte_size == 32,
                 "resize_file_size: new size is correct");

        uint8_t buffer1[16];
        read_file(file_header, buffer1, 16, 0, 16);

        for (uint32_t i = 0; i < 16; i++)
            fs_print(buffer1[i] == initial[i],
                     "resize_file_size: old data preserved after grow");

        fs_print(resize_file_size(file_header, 8),
                 "resize_file_size: shrink file to 8 bytes");

        fs_print(file_header->file_byte_size == 8,
                 "resize_file_size: shrink size correct");

        uint8_t buffer2[8];
        read_file(file_header, buffer2, 8, 0, 8);

        for (uint32_t i = 0; i < 8; i++)
            fs_print(buffer2[i] == initial[i],
                     "resize_file_size: first bytes preserved after shrink");

        fs_print(file_header->used_data_byte_size <= 8,
                 "resize_file_size: used size adjusted after shrink");

        kernel::sys::hang();
    }
} // namespace tests
