# 
# Copyright (c) 2026 Lukenar11 (Luke Matthes)
# MIT Licensed
# https://github.com/Lukenar11/MoleculeOS/blob/main/LICENSE
# 

OBJS += $(BUILD)/_start.o \
		$(BUILD)/kernel_stack.o \
		$(BUILD)/multiboot.o \
		$(BUILD)/gdt.o

$(BUILD)/%.o: arch/i386/iso/boot/%.asm
	nasm $(ASM_FLAGS) $< -o $@
