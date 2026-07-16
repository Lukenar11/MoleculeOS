/*
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
*/

ENTRY(_start)

SECTIONS 
{
    . = 1M;

    .multiboot : {
        KEEP(*(.multiboot))
    }

    .text : { 
        *(.text*) 
    }

    .rodata : { 
        *(.rodata*) 
    }

    .data : { 
        *(.data*) 
    }

    .gdt : {
        *(.gdt*)
    }
    
    .bss : { 
        *(.bss*) 
        *(COMMON) 

        . = ALIGN(8);
        heap_start = .;
        . += 265K;
        heap_end = .;
    }
}
