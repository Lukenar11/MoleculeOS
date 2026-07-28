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

#include "cmds/clear.hpp"
#include "cmds/create.hpp"
#include "cmds/echo.hpp"
#include "cmds/help.hpp"
#include "cmds/info.hpp"
#include "cmds/list.hpp"
#include "cmds/reboot.hpp"
#include "cmds/remove.hpp"
#include "cmds/shutdown.hpp"
#include "cmds/rename.hpp"
#include "cmds/copy.hpp"
