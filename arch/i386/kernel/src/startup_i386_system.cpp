/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This function is called from '_start' ('arch/i386/iso/boot/') and 
    initializes all architecture specific components
    such as the IDT and remap the PIC.

NOTES:
*/

#include "kernel/include/main.hpp"
#include "idt/idt.hpp"
#include "sys/interrupts.hpp"
#include <sal.hpp>
#include <types.hpp>
#include <io.hpp>


namespace kernel
{
    constexpr struct PIC_Mapping final {
        uint16_t port = 0;
        uint8_t value = 0;
    }
    pic_mappings[] = {
        {.port=0x0020, .value=0x11},  // init master IPC
        {.port=0x00A0, .value=0x11},  // init slave IPC
        {.port=0x0021, .value=0x20},  // set master interrupt vector offsets
        {.port=0x00A1, .value=0x28},  // set slave interrupt vector offsets
        {.port=0x0021, .value=0x04},  // tell master where the slave is connected
        {.port=0x00A1, .value=0x02},  // tell slave its cascade identity
        {.port=0x0021, .value=0x01},  // set master 8086/88 mode
        {.port=0x00A1, .value=0x01},  // set slave 8086/88 mode
        {.port=0x0021, .value=0xFD}   // enable IRQ 1 (PS/2 controller)
    };

    _API_
    extern "C"
    void 
    startup_i386_system() noexcept {
        static idt::Interrupt_Descriptor_Table idt;

        for (const auto& entry : pic_mappings)
            stdlib::byte_output(entry.port, entry.value);

        sys::enable_interrupts();

        main();
    }
} // namespace kernel
