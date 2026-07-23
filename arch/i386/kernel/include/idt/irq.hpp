/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    Each function corresponds to a specific hardware interrupt line 
    of the 'Programmable Interrupt Controller' (PIC).

NOTES:
    The internal implementation is located in 'irq.asm'.
*/

#pragma once

namespace kernel::idt::irq
{
    extern "C" {
        void irq_0();   // Timer Interrupt
        void irq_1();   // Keyboard Interrupt
        void irq_2();   // Cascade Interrupt (used internally by the PIC)
        void irq_3();   // COM2 Interrupt
        void irq_4();   // COM1 Interrupt
        void irq_5();   // LPT2 Interrupt
        void irq_6();   // Floppy Disk Interrupt
        void irq_7();   // LPT1 Interrupt
        void irq_8();   // Real-Time Clock Interrupt
        void irq_9();   // Free for peripherals (usually used for ACPI)
        void irq_10();  // Free for peripherals (usually used for ACPI)
        void irq_11();  // Free for peripherals (usually used for ACPI)
        void irq_12();  // PS/2 Mouse Interrupt
        void irq_13();  // FPU Interrupt (Math Coprocessor)
        void irq_14();  // Primary ATA Hard Disk Interrupt
        void irq_15();  // Secondary ATA Hard Disk Interrupt
    }
} // namespace kernel::irq
