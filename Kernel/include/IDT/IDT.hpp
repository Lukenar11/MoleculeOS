#pragma once

#include "utils/IDTEntry.hpp"
#include "utils/IDTDescriptorPTR.hpp"
#include "utils/IDTInitEntry.hpp"
#include "utils/helpers.hpp"
#include <Runtime/C/stdint.h>
#include <Runtime/CPP/Array.hpp>

namespace kernel::idt 
{
    class IDT final {
        private:
            runtime::Array<IDTEntry, 256> idt;
            IDTDescriptorPTR idt_ptr;

            static constexpr uint8_t CODE_SEGMENT_SELECTOR = 0x08;
            static constexpr uint8_t FLAGS = 0x8E;

        public:
            inline IDTEntry& operator[](uint32_t i) noexcept { return idt[i]; }
            inline const IDTEntry& operator[](const uint32_t i) const noexcept { return idt[i]; }

            IDT() noexcept;
            ~IDT() noexcept = default;
    };

    extern IDT idt;
} // namespace kernel::idt
