#ifndef IDT_ERRORS_H
#define IDT_ERRORS_H

#include <Runtime/C/stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// Divide Error (ISR 0)
static void test_divide_error() 
{
    static bool done = false;
    if (!done) {
        done = true;
        volatile int a = 1;
        volatile int b = 0;
        volatile int c = a / b; 
    }
}

// Breakpoint (ISR 3)
static void test_breakpoint()
{
    static bool done = false;
    if (!done) {
        done = true;
        __asm__ volatile("int $3");
    }
}

// Overflow (ISR 4)
static void test_overflow() 
{
    static bool done = false;
    if (!done) {
        done = true;
        __asm__ volatile(
            "movl $0x7FFFFFFF, %eax\n\t"
            "addl $1, %eax\n\t"
            "into\n\t"
        );
    }
}

// Invalid Opcode (ISR 6)
static void test_invalid_opcode() 
{
    static bool done = false;
    if (!done) {
        done = true;
        __asm__ volatile("ud2");
    }
}

// General Protection Fault (ISR 13)
static void general_protection_fault() 
{
    __asm__ volatile(
        "mov $0x1234, %ax\n"
        "mov %ax, %ds"
    );
}

// Custom Syscall (ISR 0x69)
static void test_syscall_0x69() 
{
    __asm__ volatile(
        "int $0x69"
    );
}
    
static inline void run_exception_tests() 
{
    // test_divide_error();
    // test_breakpoint();
    // test_overflow();
    // test_invalid_opcode();
    // general_protection_fault();
    test_syscall_0x69();
}

#ifdef __cplusplus
}
#endif

#endif // IDT_ERRORS_H
