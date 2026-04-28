#ifndef IRQ_H
#define IRQ_H

/*
    Notes: 
    Interrupt Service Routines (IRQs) for CPU exceptions (internal implementation in "IRQ.asm")

    I deliberately wrote this code in C instead of C++ 
    to be maximally compatible with the IRQ Assembly components.
*/

#ifdef __cplusplus
extern "C" {
#endif

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

#ifdef __cplusplus
}
#endif

#endif // IRQ_H
