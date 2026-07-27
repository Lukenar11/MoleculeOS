/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file defines a central API for 
    all kernel functions.

NOTES:
    Macros that have been defined based on the selected architecture are 
    compiler-specific, e.g. '__i386__' (in GCC/Clang), '_M_IX86' (in MSVC), 
    'i386' (in some cross-compilers), or may not be available at all,
    which is why custom macros are defined for this purpose, 
    e.g. '__I386__' for i386 as shown here.
*/

#pragma once

#include "kernel/include/filesys/mofs2.hpp"
#include "kernel/include/filesys/helpers.hpp"
#include "kernel/include/heap/block.hpp"

#ifdef __I386__
#   include "arch/i386/kernel/include/utils/regs.hpp"
#   include "arch/i386/kernel/include/kernel_arch_init.hpp"
#   include "arch/i386/kernel/include/sys/eflags.hpp"
#   include "arch/i386/kernel/include/sys/interrupts.hpp"
#   include "arch/i386/kernel/include/sys/halt.hpp"
#   include "arch/i386/kernel/include/sys/reboot.hpp"
#   include "arch/i386/kernel/include/sys/shutdown.hpp"
#   include "arch/i386/kernel/include/sys/panic.hpp"
#else
#   error Architecture is not supported!
#endif
