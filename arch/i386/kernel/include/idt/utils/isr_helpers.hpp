/**
LICENSE:
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
    This table is used by the 'isr_handler' to display 
    meaningful diagnostic messages during exception reporting.

NOTES:
*/

#pragma once

namespace kernel::idt::isr
{
    extern const char* exception_names[];
} // namespace kernel::irq
