#pragma once

#include <Atom/CPP/ConsoleIO.hpp>

void test_console_io() {

    atom::ConsoleIO console;
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
}
