#pragma once

#include <include/kernel/filesys.hpp>
#include <include/kernel/sys.hpp>


static void test_file_read() noexcept
{
    kernel::filesys::Handle handle;
    stdlib::Array<char, 14> buffer;
    status_t status;
        
    stdlib::console::reset();
        
    status = kernel::filesys::open_file(handle, "test", ".txt", kernel::filesys::Handle_Flags::CREATE);
    if (status != status::SUCCESS) {
        stdlib::console::put_string("\n1. open_file !status::SUCCESS\n");
        stdlib::console::put_hex(status);
    }
        
    buffer.fill('\0');
    buffer[0]  = 'H';
    buffer[1]  = 'e';
    buffer[2]  = 'l';
    buffer[3]  = 'l';
    buffer[4]  = 'o';
    buffer[5]  = ',';
    buffer[6]  = ' ';
    buffer[7]  = 'W';
    buffer[8]  = 'o';
    buffer[9]  = 'r';
    buffer[10] = 'l';
    buffer[11] = 'd';
    buffer[12] = '!';
    buffer[13] = '\0';
        
    status = kernel::filesys::write_file(handle, (const uint8_t*)buffer.data(), buffer.size(), 0, 14);
    if (status != status::SUCCESS) {
        stdlib::console::put_string("\n2. write_file !status::SUCCESS\n");
        stdlib::console::put_hex(status);
    }
        
    status = kernel::filesys::seek_file(handle, 0);
    if (status != status::SUCCESS) {
        stdlib::console::put_string("\n3. seek_file !status::SUCCESS\n");
        stdlib::console::put_hex(status);
    }
        
    buffer.fill('\0');
    status = kernel::filesys::read_file(handle, (uint8_t*)buffer.data(), buffer.size(), 0, 14);
    if (status != status::SUCCESS) {
        stdlib::console::put_string("\n4. read_file !status::SUCCESS\n");
        stdlib::console::put_hex(status);
    }
        
    stdlib::console::put_string(buffer.data());
    stdlib::console::put_char('\n');
    
    status = kernel::filesys::close_file(handle);
    if (status != status::SUCCESS) {
        stdlib::console::put_string("\n5. close_file !status::SUCCESS\n");
        stdlib::console::put_hex(status);
    }
}


namespace tests
{
    void test_file_buffer() noexcept {
        test_file_read();

        kernel::filesys::Handle handle;
        stdlib::Array<char, 14> buffer;
        status_t status;

        buffer.fill('\0');

        status = kernel::filesys::open_file(handle, "test", ".txt", kernel::filesys::Handle_Flags::READ);
        if (status != status::SUCCESS) {
            stdlib::console::put_string("\n1. open_file !status::SUCCESS\n");
            stdlib::console::put_hex(status);
        }

        buffer.fill('\0');
        status = kernel::filesys::read_file(handle, (uint8_t*)buffer.data(), buffer.size(), 0, 14);
        if (status != status::SUCCESS) {
            stdlib::console::put_string("\n4. read_file !status::SUCCESS\n");
            stdlib::console::put_hex(status);
        }

        stdlib::console::put_string(buffer.data());
        stdlib::console::put_char('\n');

        status = kernel::filesys::close_file(handle);
        if (status != status::SUCCESS) {
            stdlib::console::put_string("\n5. close_file !status::SUCCESS\n");
            stdlib::console::put_hex(status);
        }

        kernel::sys::hang();
    }
} // namespace tests
