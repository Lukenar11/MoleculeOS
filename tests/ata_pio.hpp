#pragma once

#include <types.hpp>
#include <console.hpp>
#include <array.hpp>
#include <drivers.hpp>
#include <kernel.hpp>
#include <memory.hpp>
#include <string.hpp>

namespace tests
{
    void read_sector() noexcept {
        using namespace drivers::ata;

        stdlib::console::reset();

        uint32_t lba = 0;
        int32_t sectors = 1;
        stdlib::Array<uint16_t, 256> buffer;
        uint16_t* buffer_ptr = buffer.begin();

        stdlib::console::put_string("Status before read: ");
        stdlib::console::put_hex(stdlib::byte_input(Programmable_Input_Output::status_port()));

        if (Programmable_Input_Output::run(buffer_ptr,
                                           lba, 
                                           sectors, 
                                           drivers::ata::Operations::READ) != status::SUCCESS)
            stdlib::console::put_string("\nRead Success\n\n");
        else
            stdlib::console::put_string("\nRead NOT Success\n\n");

        for (const auto& word : buffer) {
            stdlib::console::put_char(static_cast<uint8_t>(word & 0xFF));
            stdlib::console::put_char(static_cast<uint8_t>((word >> 8) & 0xFF));
        }

        stdlib::console::put_string("\n\nStatus after read: ");
        stdlib::console::put_hex(stdlib::byte_input(Programmable_Input_Output::status_port()));

        kernel::sys::hang();
    }

    void read_sector_with_heap() noexcept {
        using namespace drivers::ata;

        stdlib::console::reset();

        const uint32_t reserved_words = 265;

        uint32_t lba = 0;
        int32_t sectors = 1;
        void* temp_ptr;
        kernel::heap::allocate(temp_ptr, reserved_words << 1)
        uint16_t* ptr = static_cast<uint16_t*>(temp_ptr);

        for (uint32_t i = 0; i < reserved_words; i++)
            ptr[i] = 0;

        stdlib::console::put_string("Status before read: ");
        stdlib::console::put_hex(stdlib::byte_input(Programmable_Input_Output::status_port()));

        if (Programmable_Input_Output::run(ptr, 
                                           lba, 
                                           sectors, 
                                           drivers::ata::Operations::READ) != status::SUCCESS)
            stdlib::console::put_string("\nRead Success\n\n");
        else
            stdlib::console::put_string("\nRead NOT Success\n\n");

        for (uint32_t i = 0; i < reserved_words; i++) {
            stdlib::console::put_char(static_cast<uint8_t>(ptr[i] & 0xFF));
            stdlib::console::put_char(static_cast<uint8_t>((ptr[i] >> 8) & 0xFF));
        }

        stdlib::console::put_string("\n\nStatus after read: ");
        stdlib::console::put_hex(stdlib::byte_input(Programmable_Input_Output::status_port()));
                                      
        kernel::heap::deallocate(ptr);
        kernel::sys::hang();
    }

    void multiple_reads() noexcept {
        using namespace drivers::ata;

        stdlib::console::reset();

        stdlib::Array<uint16_t, 256> buffer;
        uint16_t* buffer_ptr = buffer.begin();
        int32_t sectors = 1;

        stdlib::console::put_string("Status before read: ");
        stdlib::console::put_hex(stdlib::byte_input(Programmable_Input_Output::status_port()));

        for (uint32_t lba = 0; lba < 16; ++lba)
            if (Programmable_Input_Output::run(buffer_ptr, 
                                               lba, 
                                               sectors, 
                                               drivers::ata::Operations::READ) != status::SUCCESS)
                stdlib::console::put_string("\nRead Success\n\n");
            else
                stdlib::console::put_string("\nRead NOT Success\n\n");

        stdlib::console::put_string("\n\nStatus after read: ");
        stdlib::console::put_hex(stdlib::byte_input(Programmable_Input_Output::status_port()));

        kernel::sys::hang();
    }

    void read_and_write_sector() noexcept {
        using namespace drivers::ata;

        stdlib::Array<uint16_t, 256> buffer;

        const char* message_to_write = "Dawn of Ashes!";
        uint32_t length;
        stdlib::string::get_string_length(length, message_to_write);
        
        buffer.fill(0);
        stdlib::memory::copy(buffer.begin(), message_to_write, length);

        uint16_t* buffer_ptr = buffer.begin();
        int32_t sectors = 1;
        uint32_t lba = 0;
        Programmable_Input_Output::run(buffer_ptr, 
                                       lba, 
                                       sectors, 
                                       drivers::ata::Operations::WRITE);
        read_sector();
    }
} // namespace tests
