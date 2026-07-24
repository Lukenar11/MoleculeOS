/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This file contains helper structures which 
    is needed for the 'IDT' initializing.

    'IDT_Descriptor_PTR' includes a from 'Intel' defined layout and 
    is used to load the 'IDT'.

    'IDT_Entry' is a from 'Intel' defined layout, 
    which represents all 'IDT' entry.

    'IDT_INIT_Entry' is a helpers structure, 
    which is used for the 'idt_init_table' to initialize the 'IDT'.

NOTES:
    The structures 'IDT_Descriptor_PTR' and 'IDT_Entry' has the 
    attribute '__attribute__((packed))', 
    which avoids byte padding because this structures must have a 
    exactly from 'Intel' defined byte layout.
*/

#include "idt/utils/idt_helpers.hpp"