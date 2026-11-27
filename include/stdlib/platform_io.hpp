/**
 * @brief Defines the public hardware-platform I/O API.
 *
 * @copyright Copyright (c) 2026 Lukenar11 (Luke Matthes)
 * @license:  https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
 * @author    Lukenar11 (Luke Matthes)
 *
 * @note This API is not architecture dependent.
 */


#pragma once

#ifdef _ARCHITECTURE_I386_
#   include "arch/i386/stdlib/include/platform_io.hpp"
#else
#   error "Architecture is not supported!"
#endif
