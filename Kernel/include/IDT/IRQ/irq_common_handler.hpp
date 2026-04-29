#pragma once

#include "Kernel/include/utils/RegisterDump.h"
#include "Kernel/include/utils/HaltSystem.h"
#include "Kernel/include/utils/io.hpp"
#include <Runtime/CPP/ConsoleIO.hpp>

static void (*irq_handlers[16])(RegisterDump*) = { nullptr };

void register_irq_handler(int irq, void (*handler)(RegisterDump*)); 

extern "C"
void irq_common_handler(RegisterDump* reg_dump);

