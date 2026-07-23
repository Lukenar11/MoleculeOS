/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file contains includes for the internal implementations 
    of all terminal commands available in MoleculeOS.

    The commands can be called by the 
    shell interpreter from a table generated at compile time, 
    where each entry contains a specific hash and a function call to the 
    command's internal implementation.

NOTES:
*/

#pragma once

#include "commands/clear.hpp"
#include "commands/create.hpp"
#include "commands/echo.hpp"
#include "commands/help.hpp"
#include "commands/info.hpp"
#include "commands/list.hpp"
#include "commands/reboot.hpp"
#include "commands/remove.hpp"
#include "commands/shutdown.hpp"
