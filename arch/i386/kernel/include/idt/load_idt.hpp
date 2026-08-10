/**
LICENSE:
	Copyright (c) 2026 Lukenar11 (Luke Matthes)
	MIT Licensed
	https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE

DESCRIPTION:
	This is a simple Assembly routine for loading the 'IDT'.
    
	This routine uses the i386 and x86/64 instruction 'lidt', 
	which tells the CPU where the 'IDT' is located and that it should be loaded.

NOTES:
	The internal implementation is located in 'load_idt.asm'.
*/

#pragma once

#include <types.hpp>

namespace kernel::idt
{
    extern "C"
    void load_idt(uint32_t);
} // namespace kernel::idt
