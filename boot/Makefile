# 
#    Copyright (c) 2026 Lukenar11 (Luke Matthes)
#    MIT Licensed
#    https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
# 

OBJS += \
	$(BUILD)/_start.o \
	$(BUILD)/kernel_stack.o \
	$(BUILD)/multiboot.o \
	$(BUILD)/gdt.o

$(BUILD)/_start.o: boot/_start.asm
	nasm $(ASMFLAGS) $< -o $@

$(BUILD)/kernel_stack.o: boot/kernel_stack.asm
	nasm $(ASMFLAGS) $< -o $@

$(BUILD)/multiboot.o: boot/multiboot.asm
	nasm $(ASMFLAGS) $< -o $@

$(BUILD)/gdt.o: boot/gdt.asm
	nasm $(ASMFLAGS) $< -o $@
