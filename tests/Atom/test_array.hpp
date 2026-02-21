#pragma once

#include "Atom/CPP/Array.hpp"

inline void test_array() {

    // Address = 0xB8000 + (line * 160)

    atom::Array<int, 10> array(0, 1, 2, 3, 4, 5, 6, 7, 8, 9);

    {
        volatile char* vga = (char*)0xB8960;
        uint32_t pos = 0;
        for (auto& element : array) {
        
            vga[pos] = ' ' + (element / 10);
            vga[pos + 1] = 0x0E;
        
            vga[pos + 2] = '0' + (element % 10);
            vga[pos + 3] = 0x0E;
        
            pos += 4;
        }
    }

    {
        volatile char* vga = (char*)0xB8A00;
        uint32_t pos = 0;
        for (uint32_t i = 0; i < array.size(); i++) {

            vga[pos] = ' ' + (array[i] / 10);
            vga[pos + 1] = 0x0E;

            vga[pos + 2] = '0' + (array[i] % 10);
            vga[pos + 3] = 0x0E;

            pos += 4;
        }
    }

    {
        volatile char* vga = (char*)0xB8AA0;
        uint32_t pos = 0;
        for (auto iter = array.begin(); iter != array.end(); iter++) {

            vga[pos] = ' ' + (*iter / 10);
            vga[pos + 1] = 0x0E;

            vga[pos + 2] = '0' + (*iter % 10);
            vga[pos + 3] = 0x0E;

            pos += 4;
        }
    }
}
