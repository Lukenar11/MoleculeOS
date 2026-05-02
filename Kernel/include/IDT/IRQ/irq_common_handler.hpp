#pragma once

#include "Kernel/include/utils/RegisterDump.h"
#include "Kernel/include/utils/HaltSystem.h"
#include "Kernel/include/utils/io.hpp"
#include <Runtime/CPP/ConsoleIO.hpp>

extern "C"
void irq_common_handler(RegisterDump* regs);
