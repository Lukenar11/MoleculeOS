ENTRY(Boot)

SECTIONS 
{
  . = 0x00100000;

  .multiboot : 
  { 
    *(.multiboot) 
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

  .bss : 
  { 
    *(.bss*) 
    *(COMMON) 
  }
}