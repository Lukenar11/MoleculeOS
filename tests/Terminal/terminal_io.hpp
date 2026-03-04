#pragma once

#include "Terminal/include/Terminal.hpp"

void test_terminal() {

    terminal::Terminal terminal;
    terminal.reset();

    terminal.put_string("Hello from C++ kernel!\n");

    terminal.print(-12345, terminal::PrintFormat::INTEGER);
    terminal.new_line();

    terminal.print(12345, terminal::PrintFormat::INTEGER);
    terminal.new_line();

    terminal.print(0xDEADBEEF, terminal::PrintFormat::HEX);
    terminal.new_line();

    terminal.print(0b101010, terminal::PrintFormat::BINARY);
    terminal.new_line();

    terminal.put_char('H');
    terminal.put_char('e');

    terminal.put_char('l');
    terminal.new_line();
    
    terminal.put_char('l');
    terminal.new_line();
    
    terminal.put_char('o');
    terminal.new_line();
}
