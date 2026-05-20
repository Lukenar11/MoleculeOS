/*
    LICENSE:
        Copyright (c) 2026 Lukenar11 (Luke Matthes)
        MIT Licensed
        https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

    DESCRIPTION:
        Initialization table for the Interrupt Descriptor Table (IDT).

        This file maps interrupt vector numbers to their corresponding
        ISR and IRQ handler entry points.

    NOTES:
        This table is used during IDT construction to populate all
        interrupt gates with the correct handler addresses.

        The handlers themselves are implemented in Assembly and exposed
        through a C-compatible interface.
*/

#include "idt_init_entry.hpp"
