/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    Each function corresponds to a specific hardware interrupt line 
    of the Programmable Interrupt Controller (PIC).

NOTES:
    Since this function is called by an assembly routine, 
    it is declared as "extern "C"" to ensure compatibility.

    The internal implementation is located in "isr.asm".
*/

#pragma once

namespace kernel::isr
{
    extern "C" {
        void isr_0();       // Divide Error
        void isr_1();       // Debug
        void isr_2();       // NMI
        void isr_3();       // Breakpoint
        void isr_4();       // Overflow
        void isr_5();       // Bound Range Exceeded
        void isr_6();       // Invalid Opcode
        void isr_7();       // Device Not Available
        void isr_8();       // Double Fault (dangerous)
        void isr_9();       // Coprocessor Segment Overrun
        void isr_10();      // Invalid TSS (error code)
        void isr_11();      // Segment Not Present (error code)
        void isr_12();      // Stack Fault (dangerous)
        void isr_13();      // General Protection Fault (error code)
        void isr_14();      // Page Fault (dangerous without paging)
        void isr_15();      // Reserved (unused)
        void isr_16();      // x87 FP Exception
        void isr_17();      // Alignment Check (error code, often unsupported)
        void isr_18();      // Machine Check
        void isr_19();      // SIMD FP Exception
        void isr_20();      // Virtualization Exception
        void isr_21();      // Control Protection Exception
        void isr_22();      // Reserved (unused)
        void isr_23();      // Reserved (unused)
        void isr_24();      // Reserved (unused)
        void isr_25();      // Reserved (unused)
        void isr_26();      // Reserved (unused)
        void isr_27();      // Reserved (unused)
        void isr_28();      // Reserved (unused)
        void isr_29();      // Reserved (unused)
        void isr_30();      // Reserved (unused)
        void isr_31();      // Reserved (unused)
        void isr_0x69();    // Syscall Interrupt (int 0x69)
    }
} // namespace kernel::isr
