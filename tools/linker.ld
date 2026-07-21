/*
    Copyright (c) 2026 Lukenar11 (Luke Matthes)
    MIT Licensed
    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
*/

ENTRY(_start)

SECTIONS 
{
    . = 1024 * 1024;

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

        . = ALIGN(16);
        stack_bottom = .;
        . += 16 * 1024;
        stack_top = .;

        . = ALIGN(16);
        heap_start = .;
        . += 2048 * 1024;
        heap_end = .;
    }
}
