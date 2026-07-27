#pragma once

#include <stdint.h>
#include <text_output.hpp>
#include <array.hpp>
#include <drivers_api.hpp>
#include <kernel_api.hpp>
#include <memory_manip.hpp>
#include <string_manip.hpp>

namespace tests
{
    void read_sector() noexcept {
        using namespace drivers::ata;

        runtime::Text_Output::reset();

        uint32_t lba = 0;
        int32_t sectors = 1;
        runtime::Array<uint16_t, 256> buffer;
        uint16_t* buffer_ptr = buffer.begin();

        runtime::Text_Output::put_string("Status before read: ");
        runtime::Text_Output::put_hex(runtime::byte_input(
                                        Programmable_Input_Output::status_port()
                                      ));

        if (Programmable_Input_Output::run(drivers::ata::Driver_Operations::READ,
                                           sectors, 
                                           buffer_ptr, 
                                           lba))
            runtime::Text_Output::put_string("\nRead Success\n\n");
        else
            runtime::Text_Output::put_string("\nRead NOT Success\n\n");

        for (const auto& word : buffer) {
            runtime::Text_Output::put_char(static_cast<uint8_t>(word & 0xFF));
            runtime::Text_Output::put_char(static_cast<uint8_t>((word >> 8) & 0xFF));
        }

        runtime::Text_Output::put_string("\n\nStatus after read: ");
        runtime::Text_Output::put_hex(runtime::byte_input(
                                        Programmable_Input_Output::status_port()
                                      ));

        kernel::sys::hang();
    }

    void read_sector_with_heap() noexcept {
        using namespace drivers::ata;

        runtime::Text_Output::reset();

        const uint32_t reserved_words = 265;

        uint32_t lba = 0;
        int32_t sectors = 1;
        uint16_t* ptr = static_cast<uint16_t*>(
            kernel::heap::Block_Allocator::allocate(reserved_words * 2)
        );

        for (uint32_t i = 0; i < reserved_words; i++)
            ptr[i] = 0;

        runtime::Text_Output::put_string("Status before read: ");
        runtime::Text_Output::put_hex(runtime::byte_input(
                                        Programmable_Input_Output::status_port()
                                      ));

        if (Programmable_Input_Output::run(drivers::ata::Driver_Operations::READ,
                                           sectors, 
                                           ptr, 
                                           lba))
            runtime::Text_Output::put_string("\nRead Success\n\n");
        else
            runtime::Text_Output::put_string("\nRead NOT Success\n\n");

        for (uint32_t i = 0; i < reserved_words; i++) {
            runtime::Text_Output::put_char(static_cast<uint8_t>(ptr[i] & 0xFF));
            runtime::Text_Output::put_char(static_cast<uint8_t>((ptr[i] >> 8) & 0xFF));
        }

        runtime::Text_Output::put_string("\n\nStatus after read: ");
        runtime::Text_Output::put_hex(runtime::byte_input(
                                        Programmable_Input_Output::status_port()
                                      ));
                                      
        kernel::heap::Block_Allocator::deallocate(ptr);
        kernel::sys::hang();
    }

    void multiple_reads() noexcept {
        using namespace drivers::ata;

        runtime::Text_Output::reset();

        runtime::Array<uint16_t, 256> buffer;
        uint16_t* buffer_ptr = buffer.begin();
        int32_t sectors = 1;

        runtime::Text_Output::put_string("Status before read: ");
        runtime::Text_Output::put_hex(runtime::byte_input(
                                        Programmable_Input_Output::status_port()
                                      ));

        for (uint32_t lba = 0; lba < 16; ++lba)
            if (Programmable_Input_Output::run(drivers::ata::Driver_Operations::READ,
                                               sectors, 
                                               buffer_ptr, 
                                               lba))
                runtime::Text_Output::put_string("\nRead Success\n\n");
            else
                runtime::Text_Output::put_string("\nRead NOT Success\n\n");

        runtime::Text_Output::put_string("\n\nStatus after read: ");
        runtime::Text_Output::put_hex(runtime::byte_input(
                                        Programmable_Input_Output::status_port()
                                      ));

        kernel::sys::hang();
    }

    void read_and_write_sector() noexcept {
        using namespace drivers::ata;

        runtime::Array<uint16_t, 256> buffer;

        const char* message_to_write = "Dawn of Ashes!";
        const uint32_t length = runtime::String_Manipulation::get_string_length(message_to_write);
        
        buffer.fill(0);
        runtime::Memory_Manipulation::copy_memory_block(buffer.begin(), 
                                                        message_to_write, 
                                                        length);

        uint16_t* buffer_ptr = buffer.begin();
        int32_t sectors = 1;
        uint32_t lba = 0;
        Programmable_Input_Output::run(drivers::ata::Driver_Operations::WRITE, 
                                       sectors, 
                                       buffer_ptr, 
                                       lba);
        read_sector();
    }
} // namespace tests
