#pragma once

#include <Runtime/CPP/ConsoleIO.hpp>

void test_console_io() {

    runtime::ConsoleIO console;
    console.reset();
    console.put_char('H');
    console.put_char('e');
    console.put_char('\n');
    console.put_char('l');
    console.put_char('l');
    console.put_char('0');
    console.put_string("\n\nHello, World!\n\n");
    console.printf("Character: %c\n", 'A');
    console.printf("Integer: %d\n", -12345);
    console.printf("Unsigned Integer: %u\n", 12345);
    console.printf("Hexadecimal: %x\n", 0xEADBEEF);
    console.printf("Binary: %b\n", 0b101010);
    console.printf("Pointer: %p\n", (void*)0xDEADBEEF);
    console.printf("Percent Sign: %%\n");
    console.printf("%c %d %u %x %b %p %%\n", 'X', -42, 42, 0x2A, 0b101010, (void*)0xDEADBEEF);
}
