#pragma once

#include <stdint.h>
#include <text_output.hpp>
#include <array.hpp>
#include <drivers_api.hpp>
#include <kernel_arch_api.hpp>

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

        if (Programmable_Input_Output::read(sectors, buffer_ptr, lba))
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

        kernel::system::hang();
    }

    void read_sector_with_heap() noexcept {
        using namespace drivers::ata;

        runtime::Text_Output::reset();

        const uint32_t reserved_bytes = 512;

        uint32_t lba = 0;
        int32_t sectors = 1;
        uint16_t* ptr = new uint16_t[reserved_bytes];

        for (uint32_t i = 0; i < reserved_bytes; i++)
            ptr[i] = 0;

        runtime::Text_Output::put_string("Status before read: ");
        runtime::Text_Output::put_hex(runtime::byte_input(
                                        Programmable_Input_Output::status_port()
                                      ));

        if (Programmable_Input_Output::read(sectors, ptr, lba))
            runtime::Text_Output::put_string("\nRead Success\n\n");
        else
            runtime::Text_Output::put_string("\nRead NOT Success\n\n");

        for (uint32_t i = 0; i < reserved_bytes; i++) {
            runtime::Text_Output::put_char(static_cast<uint8_t>(ptr[i] & 0xFF));
            runtime::Text_Output::put_char(static_cast<uint8_t>((ptr[i] >> 8) & 0xFF));
        }

        runtime::Text_Output::put_string("\n\nStatus after read: ");
        runtime::Text_Output::put_hex(runtime::byte_input(
                                        Programmable_Input_Output::status_port()
                                      ));

        delete[] ptr;
        kernel::system::hang();
    }

    void test_multiple_reads() {
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
            if (Programmable_Input_Output::read(sectors, buffer_ptr, lba))
                runtime::Text_Output::put_string("\nRead Success\n\n");
            else
                runtime::Text_Output::put_string("\nRead NOT Success\n\n");

        runtime::Text_Output::put_string("\n\nStatus after read: ");
        runtime::Text_Output::put_hex(runtime::byte_input(
                                        Programmable_Input_Output::status_port()
                                      ));

        kernel::system::hang();
    }
} // namespace tests
