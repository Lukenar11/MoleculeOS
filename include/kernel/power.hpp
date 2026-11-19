/**
 * @brief Defines the public power-management API.
 *
 * @copyright Copyright (c) 2026 Lukenar11 (Luke Matthes)
 * @license:  https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
 * @author    Lukenar11 (Luke Matthes)
 *
 * @note
 */


#pragma once

#ifdef __I386__
#   include "arch/i386/kernel/include/sys/reboot.hpp"
#   include "arch/i386/kernel/include/sys/shutdown.hpp"
#else
#   error Architecture is not supported!
#endif
