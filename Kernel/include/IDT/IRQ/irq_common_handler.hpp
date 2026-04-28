#pragma once

#include "Kernel/include/utils/RegisterDump.h"
#include "Kernel/include/utils/HaltSystem.h"
#include <Runtime/CPP/ConsoleIO.hpp>

extern "C"
void irq_common_handler(RegisterDump* reg_dump);
