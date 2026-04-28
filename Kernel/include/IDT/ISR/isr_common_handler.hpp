#pragma once

#include "utils/exception_names.h"
#include "Kernel/include/utils/RegisterDump.h"
#include "Kernel/include/utils/HaltSystem.h"
#include <Runtime/CPP/ConsoleIO.hpp>

static inline void print_reg_dump(const char* label, uint32_t value)
{
    runtime::console.put_string(label);
    runtime::console.put_hex(value);
}

extern "C"
void isr_common_handler(RegisterDump* reg_dump);
