#pragma once

#include <text_output.hpp>
#include <kernel_api.hpp>

static void fs_print(bool ok, const char* message) noexcept {
    if (ok)
        runtime::Text_Output::put_string("[ OK ] ");
    else
        runtime::Text_Output::put_string("[FAIL] ");

    runtime::Text_Output::put_string(message);
    runtime::Text_Output::put_char('\n');
}

namespace tests
{
    void create_file() {
        using namespace kernel::filesys;

        runtime::Text_Output::reset();

        I_Node* a = MoleculeOS_File_System_2::create_file("test", "txt", 32);

        fs_print(a != nullptr, "create_file: inode created");
        fs_print(a->file_byte_size == 32, "create_file: correct size");
        fs_print(a->used_data_byte_size == 0, "create_file: used size = 0");

        kernel::sys::hang();
    }

    void write_file() {
        using namespace kernel::filesys;

        runtime::Text_Output::reset();
        
        I_Node* a = MoleculeOS_File_System_2::find_file("test", "txt");

        uint8_t data1[5] = {1,2,3,4,5};
        fs_print(MoleculeOS_File_System_2::write_file(a, 0, 5, 5, data1),
                      "write_file: basic write");

        kernel::sys::hang();
    }

    void read_file() {
        using namespace kernel::filesys;

        runtime::Text_Output::reset();
        
        I_Node* a = MoleculeOS_File_System_2::find_file("test", "txt");

        uint8_t buffer[5];
        fs_print(MoleculeOS_File_System_2::read_file(a, buffer, 5, 0, 5),
                      "read_file: basic read");

        uint8_t expected[5] = {1,2,3,4,5};
        for (uint32_t i = 0; i < 5; i++)
            fs_print(buffer[i] == expected[i], "read_file: content matches");

        kernel::sys::hang();
    }

    void append_file() {
        using namespace kernel::filesys;

        runtime::Text_Output::reset();
        
        I_Node* a = MoleculeOS_File_System_2::find_file("test", "txt");

        uint8_t data2[3] = {9,9,9};
        fs_print(MoleculeOS_File_System_2::append_file(a, data2, 3),
                 "append_file: basic append");

        fs_print(a->used_data_byte_size == 3,
                 "append_file: used size updated");

        kernel::sys::hang();
    }

    void clear_file() {
        using namespace kernel::filesys;

        runtime::Text_Output::reset();
        
        I_Node* a = MoleculeOS_File_System_2::find_file("test", "txt");

        fs_print(MoleculeOS_File_System_2::clear_file(a),
                      "clear_file: cleared");

        fs_print(a->used_data_byte_size == 0,
                      "clear_file: used size reset");

        uint8_t buffer[32];
        MoleculeOS_File_System_2::read_file(a, buffer, 32, 0, 32);

        for (uint32_t i = 0; i < 32; i++)
            fs_print(buffer[i] == 0, "clear_file: all bytes zero");

        kernel::sys::hang();
    }

    void rename_file() {
        using namespace kernel::filesys;

        runtime::Text_Output::reset();
        
        fs_print(MoleculeOS_File_System_2::rename_file("test", "txt",
                                                       "log", "bin"),
                 "rename_file: renamed");

        fs_print(MoleculeOS_File_System_2::find_file("log", "bin") != nullptr,
                 "rename_file: new name exists");

        fs_print(MoleculeOS_File_System_2::find_file("test", "txt") == nullptr,
                 "rename_file: old name removed");

        kernel::sys::hang();
    }

    void copy_file() {
        using namespace kernel::filesys;

        runtime::Text_Output::reset();
        
        fs_print(MoleculeOS_File_System_2::copy_file("log", "bin",
                                                     "backup", "bin"),
                 "copy_file: copied");

        I_Node* src = MoleculeOS_File_System_2::find_file("log", "bin");
        I_Node* dst = MoleculeOS_File_System_2::find_file("backup", "bin");

        fs_print(dst != nullptr, "copy_file: backup exists");
        fs_print(dst->file_byte_size == src->file_byte_size,
                 "copy_file: size matches");
        fs_print(dst->used_data_byte_size == src->used_data_byte_size,
                 "copy_file: used size matches");

        kernel::sys::hang();
    }

    void delete_file() {
        using namespace kernel::filesys;

        runtime::Text_Output::reset();
        
        fs_print(MoleculeOS_File_System_2::delete_file("log", "bin"),
                 "delete_file: deleted");

        fs_print(MoleculeOS_File_System_2::find_file("log", "bin") == nullptr,
                 "delete_file: file removed");

        kernel::sys::hang();
    }

    void error_cases() {
        using namespace kernel::filesys;

        runtime::Text_Output::reset();
        
        I_Node* a = MoleculeOS_File_System_2::find_file("backup", "bin");

        uint8_t data1[5] = {1,2,3,4,5};

        fs_print(!MoleculeOS_File_System_2::write_file(a, 1000, 10, 10, data1),
                 "write_file: out of bounds fails");
                 
        fs_print(!MoleculeOS_File_System_2::append_file(a, data1, 1000),
                 "append_file: no space fails");

        uint8_t small_buf[2];
        fs_print(!MoleculeOS_File_System_2::read_file(a, small_buf, 2, 0, 5),
                 "read_file: buffer too small fails");

        MoleculeOS_File_System_2::create_file("x", "txt", 10);
        fs_print(!MoleculeOS_File_System_2::copy_file("x", "txt", "x", "txt"),
                 "copy_file: dest exists fails");

        kernel::sys::hang();
    }

    void resize_file_size() {
        using namespace kernel::filesys;

        runtime::Text_Output::reset();

        I_Node* inode = MoleculeOS_File_System_2::create_file("resize", "bin", 16);
        fs_print(inode != nullptr, 
                 "resize_file_size: create test file");

        uint8_t initial[16];
        for (uint32_t i = 0; i < 16; i++)
            initial[i] = i;

        MoleculeOS_File_System_2::write_file(inode, 0, 16, 16, initial);

        fs_print(MoleculeOS_File_System_2::resize_file_size(inode, 32),
                 "resize_file_size: grow file to 32 bytes");

        fs_print(inode->file_byte_size == 32,
                 "resize_file_size: new size is correct");

        uint8_t buffer1[16];
        MoleculeOS_File_System_2::read_file(inode, buffer1, 16, 0, 16);

        for (uint32_t i = 0; i < 16; i++)
            fs_print(buffer1[i] == initial[i],
                     "resize_file_size: old data preserved after grow");

        fs_print(MoleculeOS_File_System_2::resize_file_size(inode, 8),
                 "resize_file_size: shrink file to 8 bytes");

        fs_print(inode->file_byte_size == 8,
                 "resize_file_size: shrink size correct");

        uint8_t buffer2[8];
        MoleculeOS_File_System_2::read_file(inode, buffer2, 8, 0, 8);

        for (uint32_t i = 0; i < 8; i++)
            fs_print(buffer2[i] == initial[i],
                     "resize_file_size: first bytes preserved after shrink");

        fs_print(inode->used_data_byte_size <= 8,
                 "resize_file_size: used size adjusted after shrink");

        kernel::sys::hang();
    }
} // namespace tests
