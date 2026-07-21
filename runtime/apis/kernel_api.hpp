/* 
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file defines a central API for 
    all non-architecture-dependent kernel functions.

NOTES:
*/

#pragma once

#include "kernel/include/filesys/mofs.hpp"
#include "kernel/include/filesys/helpers.hpp"
#include "kernel/include/heap/block.hpp"

#ifdef __I386__
#include "arch/i386/kernel/include/utils/regs.hpp"

#include "arch/i386/kernel/include/kernel_arch_init.hpp"

#include "arch/i386/kernel/include/sys/eflags.hpp"
#include "arch/i386/kernel/include/sys/interrupts.hpp"
#include "arch/i386/kernel/include/sys/halt.hpp"
#include "arch/i386/kernel/include/sys/reboot.hpp"
#include "arch/i386/kernel/include/sys/shutdown.hpp"
#include "arch/i386/kernel/include/sys/sleep.hpp"
#include "arch/i386/kernel/include/sys/panic.hpp"
#endif
