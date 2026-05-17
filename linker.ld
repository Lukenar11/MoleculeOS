ENTRY(_start)

SECTIONS 
{
  . = 1M;

  .multiboot :
  {
    KEEP(*(.multiboot))
  }

  .text : 
  { 
    *(.text*) 
  }

  .rodata : 
  { 
    *(.rodata*) 
  }

  .data : 
  { 
    *(.data*) 
  }

  .gdt :
  {
    *(.gdt*)
  }
  
  .bss : 
  { 
    *(.bss*) 
    *(COMMON) 
  }

  . = ALIGN(8);
  heap_start = .;
  . += 128K;
  heap_end = .;
}
