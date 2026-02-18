#pragma once

typedef void (*test_fn)();

static void test_int0() {__asm__ volatile("int $0");}   
static void test_int1() {__asm__ volatile("int $1");}   
static void test_int2() {__asm__ volatile("int $2");}   
static void test_int3() {__asm__ volatile("int3");}     
static void test_int4() {__asm__ volatile("int $4");}   
static void test_int5() {__asm__ volatile("int $5");}   
static void test_int7() {__asm__ volatile("int $7");}   
static void test_int9() {__asm__ volatile("int $9");}   
static void test_int10() {__asm__ volatile("int $10");} 
static void test_int11() {__asm__ volatile("int $11");} 
static void test_int15() {__asm__ volatile("int $15");} 
static void test_int16() {__asm__ volatile("int $16");} 
static void test_int18() {__asm__ volatile("int $18");} 
static void test_int19() {__asm__ volatile("int $19");} 
static void test_int20() {__asm__ volatile("int $20");} 
static void test_int21() {__asm__ volatile("int $21");} 
static void test_int22() {__asm__ volatile("int $22");} 
static void test_int23() {__asm__ volatile("int $23");} 
static void test_int24() {__asm__ volatile("int $24");} 
static void test_int25() {__asm__ volatile("int $25");} 
static void test_int26() {__asm__ volatile("int $26");} 
static void test_int27() {__asm__ volatile("int $27");} 
static void test_int28() {__asm__ volatile("int $28");} 
static void test_int29() {__asm__ volatile("int $29");} 
static void test_int30() {__asm__ volatile("int $30");} 
static void test_int31() {__asm__ volatile("int $31");} 

static test_fn tests[] = {

    test_int0, test_int1, test_int2, test_int3,
    test_int4, test_int5, test_int7, test_int9,
    test_int10, test_int11, test_int15, test_int16,
    test_int18, test_int19, test_int20, test_int21,
    test_int22, test_int23, test_int24, test_int25,
    test_int26, test_int27, test_int28, test_int29,
    test_int30, test_int31
};

static inline void delay()
    {for (volatile uint32_t i = 0; i < 5000000; i++);}

void run_all_exception_tests() {

    for (int i = 0; i < 27; i++) {

        volatile char* vga = (char*)0xB88D0;
        vga[0] = '0' + (i / 10);    // putc('0');
        vga[2] = '0' + (i % 10);    // putc('0');
        vga[1] = vga[3] = 0x0E;     // Color: yellow

        delay();
        tests[i]();
        delay();
    }
}
