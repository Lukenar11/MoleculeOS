#pragma once

#include "utils/RegisterDump.h"
#include <Runtime/CPP/ConsoleIO.hpp>

extern "C" void isr_common_handler(RegisterDump* registers);
