/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file defines a central kernel API for all architecture-dependent functions.

    This API works by including all architecture-specific components from 'arch', 
    and preprocessor '#ifdef' cascades ensure that only the includes for the 
    respective architecture are compiled.

NOTES:
*/

#pragma once

#ifdef __I386__
#include "arch/i386/kernel/include/init_kernel.hpp"
#include "arch/i386/kernel/include/utils/register_dump.hpp"
#include "arch/i386/kernel/include/system/cpu_flags.hpp"
#include "arch/i386/kernel/include/system/enable_interrupts.hpp"
#include "arch/i386/kernel/include/system/hang.hpp"
#include "arch/i386/kernel/include/system/reboot.hpp"
#include "arch/i386/kernel/include/system/shutdown.hpp"
#include "arch/i386/kernel/include/system/sleep.hpp"
#endif
