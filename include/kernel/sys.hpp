/**
 * @brief Defines the public system API.
 *
 * @copyright Copyright (c) 2026 Lukenar11 (Luke Matthes)
 * @license:  https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
 * @author    Lukenar11 (Luke Matthes)
 *
 * @note
 */


#pragma once

#ifdef _ARCHITECTURE_I386_
#   include "arch/i386/kernel/include/utils/regs.hpp"
#   include "arch/i386/kernel/include/sys/eflags.hpp"
#   include "arch/i386/kernel/include/sys/interrupts.hpp"
#   include "arch/i386/kernel/include/sys/halt.hpp"
#   include "arch/i386/kernel/include/sys/panic.hpp"
#else
#   error Architecture is not supported!
#endif
