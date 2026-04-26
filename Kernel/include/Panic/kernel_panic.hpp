#pragma once

#include <Runtime/CPP/ConsoleIO.hpp>

extern "C" [[noreturn]] 
void kernel_panic(const char* error_message, const char* troubleshooting_message) noexcept;
