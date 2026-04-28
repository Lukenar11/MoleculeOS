#include "IDT/IDT.hpp"
#include "Heap/Heap.hpp"
#include "utils/HaltSystem.h"
#include <Runtime/CPP/ConsoleIO.hpp>

extern "C" void kernel_main() 
{
    kernel::idt::IDT idt;

    runtime::console.reset();
    runtime::console.printf(
        "%s%s%s%s%s%s%s%s%s%s%s\n",
        "__   __       _                 _         ____   _____ \n"
        "|  \\/  |     | |               | |       / __ \\ / ____|\n",
        "| \\  / | ___ | | ___  ___ _   _| | ___  | |  | | (___  \n",
        "| |\\/| |/ _ \\| |/ _ \\/ __| | | | |/ _ \\ | |  | |\\___ \\ \n",
        "| |  | | (_) | |  __/ (__| |_| | |  __/ | |__| |____) |\n",
        "|_|  |_|\\___/|_|\\___|\\___|\\__,_|_|\\___|  \\____/|_____/ \n",
        "\n",
        "Version 0.0.0 (Kernel Mode)\n",
        "\n",
        "Copyright (c) 2026 Lukenar11 (Luke Matthes)\n",
        "MIT Licensed\n",
        "https://github.com/Lukenar11/MoleculeOS\n"
    );

    runtime::console.put_string("\tTest\n\"");
    runtime::console.put_string("\tTest2\rTest3");

    HaltSystem();
}
