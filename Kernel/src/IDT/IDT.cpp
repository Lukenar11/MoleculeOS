#include "IDT/IDT.hpp"

extern "C" {

    void isr_0();   // Divide Error
    void isr_1();   // Debug
    void isr_2();   // NMI
    void isr_3();   // Breakpoint
    void isr_4();   // Overflow
    void isr_5();   // Bound Range Exceeded
    void isr_6();   // Invalid Opcode
    void isr_7();   // Device Not Available
    void isr_8();   // Double Fault (dangerous)
    void isr_9();   // Coprocessor Segment Overrun
    void isr_10();  // Invalid TSS (error code)
    void isr_11();  // Segment Not Present (error code)
    void isr_12();  // Stack Fault (dangerous)
    void isr_13();  // General Protection Fault (error code)
    void isr_14();  // Page Fault (dangerous without paging)
    void isr_15();  // Reserved (unused)
    void isr_16();  // x87 FP Exception
    void isr_17();  // Alignment Check (error code, often unsupported)
    void isr_18();  // Machine Check
    void isr_19();  // SIMD FP Exception
    void isr_20();  // Virtualization Exception
    void isr_21();  // Control Protection Exception
    void isr_22();  // Reserved (unused)
    void isr_23();  // Reserved (unused)
    void isr_24();  // Reserved (unused)
    void isr_25();  // Reserved (unused)
    void isr_26();  // Reserved (unused)
    void isr_27();  // Reserved (unused)
    void isr_28();  // Reserved (unused)
    void isr_29();  // Reserved (unused)
    void isr_30();  // Reserved (unused)
    void isr_31();  // Reserved (unused)
}   

// _init_ IDT
inline static constexpr kernel::idt::IdtInitEntry idt_init_table[32] = {

    { 0, isr_0 }, { 1, isr_1 }, { 2, isr_2 }, { 3, isr_3 }, 
    { 4, isr_4 }, { 5, isr_5 }, { 6, isr_6 }, { 7, isr_7 }, 
    { 8, isr_8 }, { 9, isr_9 }, {10, isr_10}, {11, isr_11}, 
    {12, isr_12}, {13, isr_13}, {14, isr_14}, {15, isr_15}, 
    {16, isr_16}, {17, isr_17}, {18, isr_18}, {19, isr_19}, 
    {20, isr_20}, {21, isr_21}, {22, isr_22}, {23, isr_23}, 
    {24, isr_24}, {25, isr_25}, {26, isr_26}, {27, isr_27}, 
    {28, isr_28}, {29, isr_29}, {30, isr_30}, {31, isr_31}
};

// _construct_
kernel::idt::IDT::IDT() {

    // fill IDT-Descriptor
    idt_ptr.limit = (sizeof(IdtEntry) * ENTRYS) - 1;
    idt_ptr.base = uint32_t(&idt);

    // Clear table
    for (int i = 0; i < ENTRYS; i++)
        idt[i].set_gate(0, 0, 0);

    // _build_ IDT
    for (auto& entry : idt_init_table)
        idt[entry.index].set_gate(uint32_t(entry.handler), 0x08, 0x8E);

    LoadIDT(uint32_t(&idt_ptr));
}

extern "C" void isr_common_handler() {

    static volatile char* vga = (char*)0xB88C0;
    static const char* text_message = "Error!";

    for (int i = 0; text_message[i] != '\0'; i++) {

        vga[i * 2] = text_message[i];   // putc(text_message[i]);
        vga[(i * 2) + 1] = 0x04;        // Color: red
    }
}
