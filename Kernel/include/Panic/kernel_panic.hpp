#pragma once

#include "Kernel/include/utils/HaltSystem.h"
#include <Runtime/CPP/ConsoleIO.hpp>

extern "C"
void kernel_panic(const char* error_message, const char* troubleshooting_message) noexcept;
