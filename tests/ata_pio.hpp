#pragma once

#include <types.hpp>
#include <text_output.hpp>
#include <array.hpp>
#include <drivers.hpp>
#include <kernel.hpp>
#include <memory_manip.hpp>
#include <string_manip.hpp>

namespace tests
{
    void read_sector() noexcept {
        using namespace drivers::ata;

        stdlib::Text_Output::reset();

        uint32_t lba = 0;
        int32_t sectors = 1;
        stdlib::Array<uint16_t, 256> buffer;
        uint16_t* buffer_ptr = buffer.begin();

        stdlib::Text_Output::put_string("Status before read: ");
        stdlib::Text_Output::put_hex(stdlib::byte_input(Programmable_Input_Output::status_port()));

        if (Programmable_Input_Output::run(buffer_ptr,
                                           lba, 
                                           sectors, 
                                           drivers::ata::Operations::READ) != status::SUCCESS)
            stdlib::Text_Output::put_string("\nRead Success\n\n");
        else
            stdlib::Text_Output::put_string("\nRead NOT Success\n\n");

        for (const auto& word : buffer) {
            stdlib::Text_Output::put_char(static_cast<uint8_t>(word & 0xFF));
            stdlib::Text_Output::put_char(static_cast<uint8_t>((word >> 8) & 0xFF));
        }

        stdlib::Text_Output::put_string("\n\nStatus after read: ");
        stdlib::Text_Output::put_hex(stdlib::byte_input(Programmable_Input_Output::status_port()));

        kernel::sys::hang();
    }

    void read_sector_with_heap() noexcept {
        using namespace drivers::ata;

        stdlib::Text_Output::reset();

        const uint32_t reserved_words = 265;

        uint32_t lba = 0;
        int32_t sectors = 1;
        void* temp_ptr;
        kernel::heap::Block_Allocator::allocate(temp_ptr, reserved_words << 1)
        uint16_t* ptr = static_cast<uint16_t*>(temp_ptr);

        for (uint32_t i = 0; i < reserved_words; i++)
            ptr[i] = 0;

        stdlib::Text_Output::put_string("Status before read: ");
        stdlib::Text_Output::put_hex(stdlib::byte_input(Programmable_Input_Output::status_port()));

        if (Programmable_Input_Output::run(ptr, 
                                           lba, 
                                           sectors, 
                                           drivers::ata::Operations::READ) != status::SUCCESS)
            stdlib::Text_Output::put_string("\nRead Success\n\n");
        else
            stdlib::Text_Output::put_string("\nRead NOT Success\n\n");

        for (uint32_t i = 0; i < reserved_words; i++) {
            stdlib::Text_Output::put_char(static_cast<uint8_t>(ptr[i] & 0xFF));
            stdlib::Text_Output::put_char(static_cast<uint8_t>((ptr[i] >> 8) & 0xFF));
        }

        stdlib::Text_Output::put_string("\n\nStatus after read: ");
        stdlib::Text_Output::put_hex(stdlib::byte_input(Programmable_Input_Output::status_port()));
                                      
        kernel::heap::Block_Allocator::deallocate(ptr);
        kernel::sys::hang();
    }

    void multiple_reads() noexcept {
        using namespace drivers::ata;

        stdlib::Text_Output::reset();

        stdlib::Array<uint16_t, 256> buffer;
        uint16_t* buffer_ptr = buffer.begin();
        int32_t sectors = 1;

        stdlib::Text_Output::put_string("Status before read: ");
        stdlib::Text_Output::put_hex(stdlib::byte_input(Programmable_Input_Output::status_port()));

        for (uint32_t lba = 0; lba < 16; ++lba)
            if (Programmable_Input_Output::run(buffer_ptr, 
                                               lba, 
                                               sectors, 
                                               drivers::ata::Operations::READ) != status::SUCCESS)
                stdlib::Text_Output::put_string("\nRead Success\n\n");
            else
                stdlib::Text_Output::put_string("\nRead NOT Success\n\n");

        stdlib::Text_Output::put_string("\n\nStatus after read: ");
        stdlib::Text_Output::put_hex(stdlib::byte_input(Programmable_Input_Output::status_port()));

        kernel::sys::hang();
    }

    void read_and_write_sector() noexcept {
        using namespace drivers::ata;

        stdlib::Array<uint16_t, 256> buffer;

        const char* message_to_write = "Dawn of Ashes!";
        uint32_t length;
        stdlib::String_Manipulation::get_string_length(length, message_to_write);
        
        buffer.fill(0);
        stdlib::Memory_Manipulation::copy_memory_block(buffer.begin(), 
                                                       message_to_write, 
                                                       length);

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
