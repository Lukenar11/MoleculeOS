/*
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file contains the IRQ handler table, 
    which maps IRQ numbers to their corresponding handler functions.

    The table is used by the "irq_common_handler" function to dispatch
    the appropriate handler for each IRQ.

NOTES:
*/

#include "irq/irq_handler_table.hpp"
